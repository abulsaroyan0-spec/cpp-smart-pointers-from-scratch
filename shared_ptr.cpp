#include <cstddef>
#include <utility>

template <typename T> struct ControlBlock {
		T* ptr;
		std::size_t sharedCount;
		std::size_t weakCount;

		ControlBlock(T* p) : ptr(p), sharedCount(1), weakCount(0) {
		}
};

template <typename T> class WeakPtr;

template <typename T> class SharedPtr {
	private:
		T* ptr;
		ControlBlock<T>* control;

		friend class WeakPtr<T>;

		SharedPtr(T* p, ControlBlock<T>* c) noexcept : ptr(p), control(c) {
			if (control != nullptr) {
				++control->sharedCount;
			}
		}

		void releaseCurrent() noexcept {
			if (control == nullptr) {
				ptr = nullptr;
				return;
			}

			--control->sharedCount;

			if (control->sharedCount == 0) {
				delete control->ptr;
				control->ptr = nullptr;

				if (control->weakCount == 0) {
					delete control;
				}
			}

			ptr = nullptr;
			control = nullptr;
		}

	public:
		SharedPtr() noexcept : ptr(nullptr), control(nullptr) {
		}

		SharedPtr(std::nullptr_t) noexcept : ptr(nullptr), control(nullptr) {
		}

		explicit SharedPtr(T* p) : ptr(p), control(nullptr) {
			if (p == nullptr) {
				return;
			}

			try {
				control = new ControlBlock<T>(p);
			} catch (...) {
				delete p;
				ptr = nullptr;
				throw;
			}
		}

		SharedPtr(const SharedPtr& other) noexcept
			: ptr(other.ptr), control(other.control) {
			if (control != nullptr) {
				++control->sharedCount;
			}
		}

		SharedPtr(SharedPtr&& other) noexcept
			: ptr(other.ptr), control(other.control) {
			other.ptr = nullptr;
			other.control = nullptr;
		}

		~SharedPtr() {
			releaseCurrent();
		}

		SharedPtr& operator=(const SharedPtr& other) noexcept {
			if (this == &other) {
				return *this;
			}

			releaseCurrent();

			ptr = other.ptr;
			control = other.control;

			if (control != nullptr) {
				++control->sharedCount;
			}

			return *this;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			releaseCurrent();

			ptr = other.ptr;
			control = other.control;

			other.ptr = nullptr;
			other.control = nullptr;

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

		std::size_t use_count() const noexcept {
			if (control == nullptr) {
				return 0;
			}

			return control->sharedCount;
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}

		void reset() noexcept {
			releaseCurrent();
		}

		void reset(T* newPtr) {
			if (newPtr == ptr) {
				return;
			}

			SharedPtr temp(newPtr);
			swap(temp);
		}

		void swap(SharedPtr& other) noexcept {
			using std::swap;

			swap(ptr, other.ptr);
			swap(control, other.control);
		}
};

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
	return SharedPtr<T>(new T(std::forward<Args>(args)...));
}