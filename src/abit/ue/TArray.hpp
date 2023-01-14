#pragma once

#include <cstdint>
#include <utility>

#include "abit/ue/memory.hpp"

namespace ue {

template<typename ArrayPtr, typename ElementPtr>
struct ArrayRange
{
	ArrayPtr view;

	struct Cursor
	{
		ElementPtr ptr;

		inline const auto& operator*() const { return *ptr; }
		inline auto& operator*() { return *ptr; }

		inline Cursor& operator++()
		{
			++ptr;
			return *this;
		}

		inline bool operator!=(Cursor other) { return ptr != other.ptr; }
	};

	inline Cursor begin() const { return { view->dataPtr }; }
	inline Cursor end() const { return { view->dataPtr + view->length }; }
};

/// Used for view into an existing `TArray<T>`, with the ability to modify it, but without the
/// ability to copy it or move it elsewhere.
template<typename T>
struct ViewIntoTArray
{
	T* dataPtr = nullptr;
	int32_t length = 0;
	int32_t capacity = 0;

	ViewIntoTArray(const ViewIntoTArray<T>&) = delete;
	ViewIntoTArray(ViewIntoTArray<T>&&) = delete;

	const T& operator[](size_t i) const { return dataPtr[i]; }
	T& operator[](size_t i) { return dataPtr[i]; }

	void Clear() { length = 0; }

	void Append(T&& element)
	{
		if (length + 1 > capacity) {
			capacity *= 2;
			dataPtr = reinterpret_cast<T*>(appRealloc(dataPtr, capacity));
		}
		dataPtr[length] = std::move(element);
		length += 1;
	}

	template<typename Range>
	void ExtendByCopying(const Range& range)
	{
		for (T element : range) {
			Append(std::move(element));
		}
	}

	using ConstRangeType = ArrayRange<const ViewIntoTArray<T>*, const T*>;
	inline ConstRangeType Range() const { return { this }; }

	inline typename ConstRangeType::Cursor begin() const { return Range().begin(); }
	inline typename ConstRangeType::Cursor end() const { return Range().end(); }

	using MutableRangeType = ArrayRange<ViewIntoTArray<T>*, T*>;
	inline MutableRangeType Range() { return { this }; }

	inline typename MutableRangeType::Cursor begin() { return Range().begin(); }
	inline typename MutableRangeType::Cursor end() { return Range().end(); }

private:
	ViewIntoTArray() {}
};

/// Used for owned `TArray<T>` such as those returned by functions.
template<typename T>
struct TArray
{
	T* dataPtr = nullptr;
	int32_t length = 0;
	int32_t capacity = 0;

	TArray() {}

	~TArray()
	{
		for (T& element : *this) {
			element.~T();
		}
		appFree(dataPtr);
	}

	TArray(const ViewIntoTArray<T>& view)
		: TArray()
	{
		length = view.length;
		capacity = view.length;
		dataPtr = reinterpret_cast<T*>(appMalloc(capacity * sizeof(T)));
		for (int32_t i = 0; i < view.length; ++i) {
			dataPtr[i] = view[i];
		}
	}

	TArray(TArray<T>&& other)
	{
		dataPtr = other.dataPtr;
		length = other.length;
		capacity = other.capacity;
		other.dataPtr = nullptr;
		other.length = 0;
		other.capacity = 0;
	}

	const T& operator[](size_t i) const { return dataPtr[i]; }
	T& operator[](size_t i) { return dataPtr[i]; }

	void Append(T&& element)
	{
		if (dataPtr == nullptr) {
			capacity = 8;
			dataPtr = reinterpret_cast<T*>(appMalloc(capacity * sizeof(T)));
		}
		if (length + 1 > capacity) {
			capacity *= 2;
			dataPtr = reinterpret_cast<T*>(appRealloc(dataPtr, capacity * sizeof(T)));
		}
		dataPtr[length] = std::move(element);
		length += 1;
	}

	using ConstRangeType = ArrayRange<const TArray<T>*, const T*>;
	inline ConstRangeType Range() const { return { this }; }

	inline typename ConstRangeType::Cursor begin() const { return Range().begin(); }
	inline typename ConstRangeType::Cursor end() const { return Range().end(); }

	using MutableRangeType = ArrayRange<TArray<T>*, T*>;
	inline MutableRangeType Range() { return { this }; }

	inline typename MutableRangeType::Cursor begin() { return Range().begin(); }
	inline typename MutableRangeType::Cursor end() { return Range().end(); }
};

}
