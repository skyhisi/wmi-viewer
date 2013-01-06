#include "qtcominterop.hpp"

#include <new>
#include "wmibase.hpp"

BStrScoped::BStrScoped(const QString& str) :
	bs(SysAllocString(reinterpret_cast<const OLECHAR*>(str.utf16())))
{}

BStrScoped::~BStrScoped() throw()
{
	SysFreeString(bs);
}

////////////////////////////////////////////////////////////////////////////////

SafeArrayScoped::SafeArrayScoped(SAFEARRAY* array, bool takeOwnership) :
	_arr(array),
	own(takeOwnership)
{}

SafeArrayScoped::SafeArrayScoped(VARTYPE vt, ULONG size) :
	_arr(0),
	own(true)
{
	SAFEARRAYBOUND bounds;
	bounds.lLbound = 0;
	bounds.cElements = size;
	
	_arr = SafeArrayCreate(vt, 1, &bounds);
	if (_arr == 0)
		throw std::bad_alloc();
}

SafeArrayScoped::~SafeArrayScoped() throw ()
{
	if (_arr != 0 && own)
	{
		SafeArrayDestroy(_arr);
	}
}

unsigned int SafeArrayScoped::dimensions() const
{
	if (_arr == 0) return 0;
	return SafeArrayGetDim(_arr);
}

long SafeArrayScoped::lowerBound(unsigned int dim) const
{
	if (_arr == 0) return -1;
	long bound;
	HRESULT hr = SafeArrayGetLBound(_arr, dim, &bound);
	if (FAILED(hr))
		qWarningFromHresult(hr, "SafeArrayScoped::lowerBound");
	return (FAILED(hr)) ? -1 : bound;
}

long SafeArrayScoped::upperBound(unsigned int dim) const
{
	if (_arr == 0) return -1;
	long bound;
	HRESULT hr = SafeArrayGetUBound(_arr, dim, &bound);
	if (FAILED(hr))
		qWarningFromHresult(hr, "SafeArrayScoped::upperBound");
	return (FAILED(hr)) ? -1 : bound;
}

VARTYPE SafeArrayScoped::vartype() const
{
	if (_arr == 0) return VT_UNKNOWN;
	VARTYPE type = VT_UNKNOWN;
	SafeArrayGetVartype(_arr, &type);
	return type;
}

QStringList SafeArrayScoped::toStringList() const
{
	if (vartype() != VT_BSTR || dimensions() != 1)
		return QStringList();

	QStringList list;
	BSTR* bslist = 0;
	if (FAILED(SafeArrayAccessData(_arr, reinterpret_cast<void**>(&bslist))))
		return QStringList();
	
	for (unsigned long i = 0; i < _arr->rgsabound[0].cElements; ++i)
	{
		list << QString::fromUtf16(reinterpret_cast<const unsigned short*>(bslist[i]));
	}
	SafeArrayUnaccessData(_arr);
	return list;
}

QVariantList SafeArrayScoped::toList() const
{
	if (dimensions() != 1)
		return QVariantList();
	
	const VARTYPE t = vartype();
	if (t != VT_I4 || t != VT_BSTR)
		return QVariantList();

	QVariantList list;
	void* vslist = 0;
	if (FAILED(SafeArrayAccessData(_arr, &vslist)))
		return QVariantList();
	
	for (unsigned long i = 0; i < _arr->rgsabound[0].cElements; ++i)
	{
		switch(t)
		{
			case VT_I4: list << QVariant::fromValue(reinterpret_cast<int*>(vslist)[i]); break;
			case VT_BSTR: list << QVariant::fromValue(QString::fromUtf16(&reinterpret_cast<const unsigned short*>(vslist)[i])); break;
			default: list << QVariant(); break;
		};
	}
	SafeArrayUnaccessData(_arr);
	return list;
}



////////////////////////////////////////////////////////////////////////////////


QVariant fromComVariant(const VARIANT& v)
{
	switch (v.vt)
	{
		case VT_EMPTY: return QVariant();
		case VT_NULL: return QVariant(QMetaType::VoidStar, (void*)0);
		case VT_I1: return QVariant::fromValue(v.cVal);
		case VT_UI1: return QVariant::fromValue(v.bVal);
		case VT_I2: return QVariant::fromValue(v.iVal);
		case VT_UI2: return QVariant::fromValue(v.uiVal);
		case VT_I4: return QVariant::fromValue(v.lVal);
		case VT_UI4: return QVariant::fromValue(v.ulVal);
		case VT_INT: return QVariant::fromValue(v.intVal);
		case VT_UINT: return QVariant::fromValue(v.uintVal);
		case VT_I8: return QVariant::fromValue(v.llVal);
		case VT_UI8: return QVariant::fromValue(v.ullVal);
		case VT_R4: return QVariant::fromValue(v.fltVal);
		case VT_BOOL: return QVariant(bool(v.boolVal == -1));
		case VT_UNKNOWN: return QVariant::fromValue(ComUnknown(*v.ppunkVal, false));
		case VT_ERROR: return QVariant::fromValue(v.scode);
		case VT_CY: return QVariant::fromValue(v.cyVal.int64);
		case VT_DATE: return QVariant(QDate(30,12,1899).addDays(v.date));
		case VT_BSTR: return QVariant(QString::fromUtf16(reinterpret_cast<const unsigned short*>(v.bstrVal)));
		
		case VT_BSTR | VT_ARRAY: return QVariant(SafeArrayScoped(v.parray, false).toStringList());
		case VT_I4 | VT_ARRAY: return QVariant(SafeArrayScoped(v.parray, false).toList());
	}
	qWarning() << "UNKNOWN COM VARIANT TYPE" << v.vt;
	return QVariant();
}

bool toComVariant(const QVariant& in, VARIANT& out)
{
	VariantInit(&out);
	if (!in.isValid())
		return false;
		
	HRESULT hr;
		
	if (in.isNull())
	{
		hr = VariantChangeType(&out, &out, 0, VT_NULL);
		if (hr != S_OK)
		{
			qWarning() << "toComVariant: Error setting NULL";
			return false;
		}
		return true;
	}
	
#define SETV(QTTYPE, COMTYPE, ATTR, VALUE) case QTTYPE: out.ATTR = (VALUE); hr = VariantChangeType(&out, &out, 0, COMTYPE); break
	switch (in.userType())
	{
		SETV(QMetaType::Bool, VT_BOOL, boolVal, (in.toBool() ? -1 : 0));
		SETV(QMetaType::Int, VT_I4, lVal, in.value<int>());
		SETV(QMetaType::QString, VT_BSTR, bstrVal, SysAllocString(reinterpret_cast<const OLECHAR*>(in.toString().utf16())));
		
		default:
			qWarning() << "UNKNOWN QT VARIANT TYPE" << in.typeName() << in.userType();
			return false;
	}
#undef SETV
	if (hr != S_OK)
	{
		qWarning() << "toComVariant: Error setting type";
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

const char* HResultToName(HRESULT hr)
{
	switch (hr)
	{
		case WBEM_NO_ERROR: return "NO_ERROR";
		case WBEM_S_FALSE: return "S_FALSE";
		case WBEM_S_ALREADY_EXISTS: return "S_ALREADY_EXISTS";
		case WBEM_S_RESET_TO_DEFAULT: return "S_RESET_TO_DEFAULT";
		case WBEM_S_DIFFERENT: return "S_DIFFERENT";
		case WBEM_S_TIMEDOUT: return "S_TIMEDOUT";
		case WBEM_S_NO_MORE_DATA: return "S_NO_MORE_DATA";
		case WBEM_S_OPERATION_CANCELLED: return "S_OPERATION_CANCELLED";
		case WBEM_S_PENDING: return "S_PENDING";
		case WBEM_S_DUPLICATE_OBJECTS: return "S_DUPLICATE_OBJECTS";
		case WBEM_S_ACCESS_DENIED: return "S_ACCESS_DENIED";
		case WBEM_S_PARTIAL_RESULTS: return "S_PARTIAL_RESULTS";
		case WBEM_S_SOURCE_NOT_AVAILABLE: return "S_SOURCE_NOT_AVAILABLE";
		case WBEM_E_FAILED: return "E_FAILED";
		case WBEM_E_NOT_FOUND: return "E_NOT_FOUND";
		case WBEM_E_ACCESS_DENIED: return "E_ACCESS_DENIED";
		case WBEM_E_PROVIDER_FAILURE: return "E_PROVIDER_FAILURE";
		case WBEM_E_TYPE_MISMATCH: return "E_TYPE_MISMATCH";
		case WBEM_E_OUT_OF_MEMORY: return "E_OUT_OF_MEMORY";
		case WBEM_E_INVALID_CONTEXT: return "E_INVALID_CONTEXT";
		case WBEM_E_INVALID_PARAMETER: return "E_INVALID_PARAMETER";
		case WBEM_E_NOT_AVAILABLE: return "E_NOT_AVAILABLE";
		case WBEM_E_CRITICAL_ERROR: return "E_CRITICAL_ERROR";
		case WBEM_E_INVALID_STREAM: return "E_INVALID_STREAM";
		case WBEM_E_NOT_SUPPORTED: return "E_NOT_SUPPORTED";
		case WBEM_E_INVALID_SUPERCLASS: return "E_INVALID_SUPERCLASS";
		case WBEM_E_INVALID_NAMESPACE: return "E_INVALID_NAMESPACE";
		case WBEM_E_INVALID_OBJECT: return "E_INVALID_OBJECT";
		case WBEM_E_INVALID_CLASS: return "E_INVALID_CLASS";
		case WBEM_E_PROVIDER_NOT_FOUND: return "E_PROVIDER_NOT_FOUND";
		case WBEM_E_INVALID_PROVIDER_REGISTRATION: return "E_INVALID_PROVIDER_REGISTRATION";
		case WBEM_E_PROVIDER_LOAD_FAILURE: return "E_PROVIDER_LOAD_FAILURE";
		case WBEM_E_INITIALIZATION_FAILURE: return "E_INITIALIZATION_FAILURE";
		case WBEM_E_TRANSPORT_FAILURE: return "E_TRANSPORT_FAILURE";
		case WBEM_E_INVALID_OPERATION: return "E_INVALID_OPERATION";
		case WBEM_E_INVALID_QUERY: return "E_INVALID_QUERY";
		case WBEM_E_INVALID_QUERY_TYPE: return "E_INVALID_QUERY_TYPE";
		case WBEM_E_ALREADY_EXISTS: return "E_ALREADY_EXISTS";
		case WBEM_E_OVERRIDE_NOT_ALLOWED: return "E_OVERRIDE_NOT_ALLOWED";
		case WBEM_E_PROPAGATED_QUALIFIER: return "E_PROPAGATED_QUALIFIER";
		case WBEM_E_PROPAGATED_PROPERTY: return "E_PROPAGATED_PROPERTY";
		case WBEM_E_UNEXPECTED: return "E_UNEXPECTED";
		case WBEM_E_ILLEGAL_OPERATION: return "E_ILLEGAL_OPERATION";
		case WBEM_E_CANNOT_BE_KEY: return "E_CANNOT_BE_KEY";
		case WBEM_E_INCOMPLETE_CLASS: return "E_INCOMPLETE_CLASS";
		case WBEM_E_INVALID_SYNTAX: return "E_INVALID_SYNTAX";
		case WBEM_E_NONDECORATED_OBJECT: return "E_NONDECORATED_OBJECT";
		case WBEM_E_READ_ONLY: return "E_READ_ONLY";
		case WBEM_E_PROVIDER_NOT_CAPABLE: return "E_PROVIDER_NOT_CAPABLE";
		case WBEM_E_CLASS_HAS_CHILDREN: return "E_CLASS_HAS_CHILDREN";
		case WBEM_E_CLASS_HAS_INSTANCES: return "E_CLASS_HAS_INSTANCES";
		case WBEM_E_QUERY_NOT_IMPLEMENTED: return "E_QUERY_NOT_IMPLEMENTED";
		case WBEM_E_ILLEGAL_NULL: return "E_ILLEGAL_NULL";
		case WBEM_E_INVALID_QUALIFIER_TYPE: return "E_INVALID_QUALIFIER_TYPE";
		case WBEM_E_INVALID_PROPERTY_TYPE: return "E_INVALID_PROPERTY_TYPE";
		case WBEM_E_VALUE_OUT_OF_RANGE: return "E_VALUE_OUT_OF_RANGE";
		case WBEM_E_CANNOT_BE_SINGLETON: return "E_CANNOT_BE_SINGLETON";
		case WBEM_E_INVALID_CIM_TYPE: return "E_INVALID_CIM_TYPE";
		case WBEM_E_INVALID_METHOD: return "E_INVALID_METHOD";
		case WBEM_E_INVALID_METHOD_PARAMETERS: return "E_INVALID_METHOD_PARAMETERS";
		case WBEM_E_SYSTEM_PROPERTY: return "E_SYSTEM_PROPERTY";
		case WBEM_E_INVALID_PROPERTY: return "E_INVALID_PROPERTY";
		case WBEM_E_CALL_CANCELLED: return "E_CALL_CANCELLED";
		case WBEM_E_SHUTTING_DOWN: return "E_SHUTTING_DOWN";
		case WBEM_E_PROPAGATED_METHOD: return "E_PROPAGATED_METHOD";
		case WBEM_E_UNSUPPORTED_PARAMETER: return "E_UNSUPPORTED_PARAMETER";
		case WBEM_E_MISSING_PARAMETER_ID: return "E_MISSING_PARAMETER_ID";
		case WBEM_E_INVALID_PARAMETER_ID: return "E_INVALID_PARAMETER_ID";
		case WBEM_E_NONCONSECUTIVE_PARAMETER_IDS: return "E_NONCONSECUTIVE_PARAMETER_IDS";
		case WBEM_E_PARAMETER_ID_ON_RETVAL: return "E_PARAMETER_ID_ON_RETVAL";
		case WBEM_E_INVALID_OBJECT_PATH: return "E_INVALID_OBJECT_PATH";
		case WBEM_E_OUT_OF_DISK_SPACE: return "E_OUT_OF_DISK_SPACE";
		case WBEM_E_BUFFER_TOO_SMALL: return "E_BUFFER_TOO_SMALL";
		case WBEM_E_UNSUPPORTED_PUT_EXTENSION: return "E_UNSUPPORTED_PUT_EXTENSION";
		case WBEM_E_UNKNOWN_OBJECT_TYPE: return "E_UNKNOWN_OBJECT_TYPE";
		case WBEM_E_UNKNOWN_PACKET_TYPE: return "E_UNKNOWN_PACKET_TYPE";
		case WBEM_E_MARSHAL_VERSION_MISMATCH: return "E_MARSHAL_VERSION_MISMATCH";
		case WBEM_E_MARSHAL_INVALID_SIGNATURE: return "E_MARSHAL_INVALID_SIGNATURE";
		case WBEM_E_INVALID_QUALIFIER: return "E_INVALID_QUALIFIER";
		case WBEM_E_INVALID_DUPLICATE_PARAMETER: return "E_INVALID_DUPLICATE_PARAMETER";
		case WBEM_E_TOO_MUCH_DATA: return "E_TOO_MUCH_DATA";
		case WBEM_E_SERVER_TOO_BUSY: return "E_SERVER_TOO_BUSY";
		case WBEM_E_INVALID_FLAVOR: return "E_INVALID_FLAVOR";
		case WBEM_E_CIRCULAR_REFERENCE: return "E_CIRCULAR_REFERENCE";
		case WBEM_E_UNSUPPORTED_CLASS_UPDATE: return "E_UNSUPPORTED_CLASS_UPDATE";
		case WBEM_E_CANNOT_CHANGE_KEY_INHERITANCE: return "E_CANNOT_CHANGE_KEY_INHERITANCE";
		case WBEM_E_CANNOT_CHANGE_INDEX_INHERITANCE: return "E_CANNOT_CHANGE_INDEX_INHERITANCE";
		case WBEM_E_TOO_MANY_PROPERTIES: return "E_TOO_MANY_PROPERTIES";
		case WBEM_E_UPDATE_TYPE_MISMATCH: return "E_UPDATE_TYPE_MISMATCH";
		case WBEM_E_UPDATE_OVERRIDE_NOT_ALLOWED: return "E_UPDATE_OVERRIDE_NOT_ALLOWED";
		case WBEM_E_UPDATE_PROPAGATED_METHOD: return "E_UPDATE_PROPAGATED_METHOD";
		case WBEM_E_METHOD_NOT_IMPLEMENTED: return "E_METHOD_NOT_IMPLEMENTED";
		case WBEM_E_METHOD_DISABLED: return "E_METHOD_DISABLED";
		case WBEM_E_REFRESHER_BUSY: return "E_REFRESHER_BUSY";
		case WBEM_E_UNPARSABLE_QUERY: return "E_UNPARSABLE_QUERY";
		case WBEM_E_NOT_EVENT_CLASS: return "E_NOT_EVENT_CLASS";
		case WBEM_E_MISSING_GROUP_WITHIN: return "E_MISSING_GROUP_WITHIN";
		case WBEM_E_MISSING_AGGREGATION_LIST: return "E_MISSING_AGGREGATION_LIST";
		case WBEM_E_PROPERTY_NOT_AN_OBJECT: return "E_PROPERTY_NOT_AN_OBJECT";
		case WBEM_E_AGGREGATING_BY_OBJECT: return "E_AGGREGATING_BY_OBJECT";
		case WBEM_E_UNINTERPRETABLE_PROVIDER_QUERY: return "E_UNINTERPRETABLE_PROVIDER_QUERY";
		case WBEM_E_BACKUP_RESTORE_WINMGMT_RUNNING: return "E_BACKUP_RESTORE_WINMGMT_RUNNING";
		case WBEM_E_QUEUE_OVERFLOW: return "E_QUEUE_OVERFLOW";
		case WBEM_E_PRIVILEGE_NOT_HELD: return "E_PRIVILEGE_NOT_HELD";
		case WBEM_E_INVALID_OPERATOR: return "E_INVALID_OPERATOR";
		case WBEM_E_LOCAL_CREDENTIALS: return "E_LOCAL_CREDENTIALS";
		case WBEM_E_CANNOT_BE_ABSTRACT: return "E_CANNOT_BE_ABSTRACT";
		case WBEM_E_AMENDED_OBJECT: return "E_AMENDED_OBJECT";
		case WBEM_E_CLIENT_TOO_SLOW: return "E_CLIENT_TOO_SLOW";
		case WBEM_E_NULL_SECURITY_DESCRIPTOR: return "E_NULL_SECURITY_DESCRIPTOR";
		case WBEM_E_TIMED_OUT: return "E_TIMED_OUT";
		case WBEM_E_INVALID_ASSOCIATION: return "E_INVALID_ASSOCIATION";
		case WBEM_E_AMBIGUOUS_OPERATION: return "E_AMBIGUOUS_OPERATION";
		case WBEM_E_QUOTA_VIOLATION: return "E_QUOTA_VIOLATION";
		case WBEM_E_RESERVED_001: return "E_RESERVED_001";
		case WBEM_E_RESERVED_002: return "E_RESERVED_002";
		case WBEM_E_UNSUPPORTED_LOCALE: return "E_UNSUPPORTED_LOCALE";
		case WBEM_E_HANDLE_OUT_OF_DATE: return "E_HANDLE_OUT_OF_DATE";
		case WBEM_E_CONNECTION_FAILED: return "E_CONNECTION_FAILED";
		case WBEM_E_INVALID_HANDLE_REQUEST: return "E_INVALID_HANDLE_REQUEST";
		case WBEM_E_PROPERTY_NAME_TOO_WIDE: return "E_PROPERTY_NAME_TOO_WIDE";
		case WBEM_E_CLASS_NAME_TOO_WIDE: return "E_CLASS_NAME_TOO_WIDE";
		case WBEM_E_METHOD_NAME_TOO_WIDE: return "E_METHOD_NAME_TOO_WIDE";
		case WBEM_E_QUALIFIER_NAME_TOO_WIDE: return "E_QUALIFIER_NAME_TOO_WIDE";
		case WBEM_E_RERUN_COMMAND: return "E_RERUN_COMMAND";
		case WBEM_E_DATABASE_VER_MISMATCH: return "E_DATABASE_VER_MISMATCH";
		case WBEM_E_VETO_DELETE: return "E_VETO_DELETE";
		case WBEM_E_VETO_PUT: return "E_VETO_PUT";
		case WBEM_E_INVALID_LOCALE: return "E_INVALID_LOCALE";
		case WBEM_E_PROVIDER_SUSPENDED: return "E_PROVIDER_SUSPENDED";
		case WBEM_E_SYNCHRONIZATION_REQUIRED: return "E_SYNCHRONIZATION_REQUIRED";
		case WBEM_E_NO_SCHEMA: return "E_NO_SCHEMA";
		case WBEM_E_PROVIDER_ALREADY_REGISTERED: return "E_PROVIDER_ALREADY_REGISTERED";
		case WBEM_E_PROVIDER_NOT_REGISTERED: return "E_PROVIDER_NOT_REGISTERED";
		case WBEM_E_FATAL_TRANSPORT_ERROR: return "E_FATAL_TRANSPORT_ERROR";
		case WBEM_E_ENCRYPTED_CONNECTION_REQUIRED: return "E_ENCRYPTED_CONNECTION_REQUIRED";
		case WBEM_E_PROVIDER_TIMED_OUT: return "E_PROVIDER_TIMED_OUT";
		case WBEM_E_NO_KEY: return "E_NO_KEY";
		case WBEM_E_PROVIDER_DISABLED: return "E_PROVIDER_DISABLED";
		default: return "UNKNOWN_ERROR";
	}
}

void qWarningFromHresult(HRESULT hr, const char* msg)
{
	qWarning() << "WMI Error:" << HResultToName(hr) << msg;
}


