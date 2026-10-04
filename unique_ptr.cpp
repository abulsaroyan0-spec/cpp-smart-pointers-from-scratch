#include <cstddef>
#include <utility>

template <typename T>
struct DefaultDeleter {
	void operator()(T* ptr) const noexcept {
		delete ptr;
	}
};

template <typename T>
struct DefaultDeleter<T[]> {
	void operator()(T* ptr) const noexcept {
		delete[] ptr;
	}
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
	private:
		T* ptr;
		Deleter deleter;

	public:
		UniquePtr() noexcept
			: ptr(nullptr), deleter() {
		}

		UniquePtr(std::nullptr_t) noexcept
			: ptr(nullptr), deleter() {
		}

		explicit UniquePtr(T* p) noexcept
			: ptr(p), deleter() {
		}

		UniquePtr(T* p, Deleter d) noexcept
			: ptr(p), deleter(std::move(d)) {
		}

		~UniquePtr() {
			if (ptr != nullptr) {
				deleter(ptr);
			}
		}

		UniquePtr(const UniquePtr&) = delete;

		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept
			: ptr(other.ptr),
			  deleter(std::move(other.deleter)) {
			other.ptr = nullptr;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			if (ptr != nullptr) {
				deleter(ptr);
			}

			ptr = other.ptr;
			deleter = std::move(other.deleter);

			other.ptr = nullptr;

			return *this;
		}

		UniquePtr& operator=(std::nullptr_t) {
			reset();
			return *this;
		}

		T& operator*() const {
			return *ptr;
		}

		T* operator->() const noexcept {
			return ptr;
		}

		T* get() const noexcept {
			return ptr;
		}

		Deleter& getDeleter() noexcept {
			return deleter;
		}

		const Deleter& getDeleter() const noexcept {
			return deleter;
		}

		T* release() noexcept {
			T* oldPtr = ptr;
			ptr = nullptr;

			return oldPtr;
		}

		void reset(T* newPtr = nullptr) {
			if (ptr == newPtr) {
				return;
			}

			T* oldPtr = ptr;
			ptr = newPtr;

			if (oldPtr != nullptr) {
				deleter(oldPtr);
			}
		}

		void swap(UniquePtr& other) noexcept {
			using std::swap;

			swap(ptr, other.ptr);
			swap(deleter, other.deleter);
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
	private:
		T* ptr;
		Deleter deleter;

	public:
		UniquePtr() noexcept
			: ptr(nullptr), deleter() {
		}

		UniquePtr(std::nullptr_t) noexcept
			: ptr(nullptr), deleter() {
		}

		explicit UniquePtr(T* p) noexcept
			: ptr(p), deleter() {
		}

		UniquePtr(T* p, Deleter d) noexcept
			: ptr(p), deleter(std::move(d)) {
		}

		~UniquePtr() {
			if (ptr != nullptr) {
				deleter(ptr);
			}
		}

		UniquePtr(const UniquePtr&) = delete;

		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept
			: ptr(other.ptr),
			  deleter(std::move(other.deleter)) {
			other.ptr = nullptr;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			if (ptr != nullptr) {
				deleter(ptr);
			}

			ptr = other.ptr;
			deleter = std::move(other.deleter);

			other.ptr = nullptr;

			return *this;
		}

		UniquePtr& operator=(std::nullptr_t) {
			reset();
			return *this;
		}

		T& operator[](std::size_t index) const {
			return ptr[index];
		}

		T* get() const noexcept {
			return ptr;
		}

		Deleter& getDeleter() noexcept {
			return deleter;
		}

		const Deleter& getDeleter() const noexcept {
			return deleter;
		}

		T* release() noexcept {
			T* oldPtr = ptr;
			ptr = nullptr;

			return oldPtr;
		}

		void reset(T* newPtr = nullptr) {
			if (ptr == newPtr) {
				return;
			}

			T* oldPtr = ptr;
			ptr = newPtr;

			if (oldPtr != nullptr) {
				deleter(oldPtr);
			}
		}

		void swap(UniquePtr& other) noexcept {
			using std::swap;

			swap(ptr, other.ptr);
			swap(deleter, other.deleter);
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}
};

template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
	return UniquePtr<T>(
		new T(std::forward<Args>(args)...)
	);
}

template <typename T>
UniquePtr<T[]> makeUniqueArray(std::size_t size) {
	return UniquePtr<T[]>(
		new T[size]()
	);
}