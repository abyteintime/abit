#pragma once

#include <cstdint>

namespace ue {

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

	struct RangeT
	{
		const ViewIntoTArray<T>* view;

		struct Cursor
		{
			const T* ptr;

			inline const T& operator*() const { return *ptr; }

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

	inline RangeT Range() const { return { this }; }

private:
	ViewIntoTArray() {}
};

}
