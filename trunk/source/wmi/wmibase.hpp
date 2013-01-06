#ifndef _WMIVIEWER_WMIBASE_HPP_
#define _WMIVIEWER_WMIBASE_HPP_

#ifdef WIN32
#  ifdef qtwmiwrapper_EXPORTS
#    define EXPORT __declspec(dllexport)
#  else
#    define EXPORT __declspec(dllimport)
#  endif
#else
#  define EXPORT
#endif

#include <stdexcept>
#include <windows.h>
#include <wbemidl.h>
#include "qtcominterop.hpp"

template <class T>
class ComPointer
{
	public:
		virtual ~ComPointer() throw() { release(); }
		
		T* get() const throw() { return value; }
		T* operator->() const throw() { return value; }
		void release() throw() { reset(0); }
		void reset(T* v, bool take = true) throw() {if (value != 0) value->Release(); value = v; if (!take && value != 0) value->AddRef(); }
		bool valid() const throw() { return (value != 0); }
		
	protected:
		ComPointer(T* v, bool take) : value(v)
		{ if (!take && value != 0) value->AddRef(); }
		
		ComPointer(const ComPointer& other) : value(other.value)
		{ if (value != 0) value->AddRef(); }

		ComPointer& operator=(const ComPointer& other)
		{
			if (value != 0) value->Release();
			value = other.value;
			if (value != 0) value->AddRef();
			return *this;
		}
		
	private:
		T* value;
};

class ComUnknown : public ComPointer<IUnknown>
{
	public:
		ComUnknown() : ComPointer<IUnknown>(0, true) {}
		ComUnknown(IUnknown* v, bool take) : ComPointer<IUnknown>(v, take) {}
		// ComUnknown(const ComUnknown& other) : ComPointer<IUnknown>(other) {}
		// ComUnknown& operator=(const ComUnknown& other) { ComPointer<IUnknown>::operator=(other); return *this; }
};

Q_DECLARE_METATYPE(ComUnknown)


template <class T>
class WmiCloneable : public virtual ComPointer<T>
{
	public:
		WmiCloneable clone() const
		{
			if (!valid()) return WmiCloneable(0, true);
			T* newValue = 0;
			HRESULT hr = get()->Clone(&newValue);
			if (FAILED(hr))
			{
				qWarningFromHresult(hr, "Clone Error");
				return WmiCloneable(0, true);
			}
			return WmiCloneable(newValue, true);
		}
		
	protected:
		WmiCloneable(T* v, bool take) : ComPointer<T>(v, take) {}
		
};


template <class T>
class WmiComparable : public virtual ComPointer<T>
{
	public:
		bool equal(const WmiComparable<T>& other) const
		{
			if (!valid()) return false;
			if (!other.valid()) return false;
			HRESULT hr = get()->CompareTo(WBEM_COMPARISON_INCLUDE_ALL, other.get());
			if (hr == WBEM_S_SAME) return true;
			if (hr == WBEM_S_DIFFERENT) return false;
			qWarningFromHresult(hr, "Compare Error");
			return false;
		}
	
	protected:
		WmiComparable(T* v, bool take) : ComPointer<T>(v, take) {}
};


#endif
