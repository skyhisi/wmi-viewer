#ifndef _WMIVIEWER_QTCOMINTEROP_HPP_
#define _WMIVIEWER_QTCOMINTEROP_HPP_

#include <QtCore>
#include <wbemidl.h>

class BStrScoped
{
	public:
		explicit BStrScoped(const QString& str);
		~BStrScoped() throw();
		
		operator BSTR() const {return bs;}
		
	private:
		BSTR bs;
		BStrScoped(const BStrScoped&);
		BStrScoped& operator=(const BStrScoped&);
};

class SafeArrayScoped
{
	public:
		explicit SafeArrayScoped(SAFEARRAY* array, bool takeOwnership = true);
		explicit SafeArrayScoped(VARTYPE vt, ULONG size);
		~SafeArrayScoped() throw ();
		
		SAFEARRAY* get() const { return _arr; }
		SAFEARRAY** ptr() {return &_arr; }
		
		unsigned int dimensions() const;
		long lowerBound(unsigned int dim = 0) const;
		long upperBound(unsigned int dim = 0) const;
		VARTYPE vartype() const;

		QStringList toStringList() const;
		QVariantList toList() const;
		
		
	private:
		SAFEARRAY* _arr;
		bool own;
		SafeArrayScoped(const SafeArrayScoped&);
		SafeArrayScoped& operator=(const SafeArrayScoped&);
};

QVariant fromComVariant(const VARIANT& v);

const char* HResultToName(HRESULT hr);

void qWarningFromHresult(HRESULT hr, const char* msg);

#endif
