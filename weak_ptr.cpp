template <typename T> class WeakPtr {
	private:
		T* ptr;
		ControlBlock<T>* control;

		void releaseCurrent() noexcept {
			if (control == nullptr) {
				ptr = nullptr;
				return;
			}

			--control->weakCount;

			if (control->weakCount == 0 && control->sharedCount == 0) {
				delete control;
			}

			ptr = nullptr;
			control = nullptr;
		}

	public:
		WeakPtr() noexcept : ptr(nullptr), control(nullptr) {
		}

		WeakPtr(std::nullptr_t) noexcept : ptr(nullptr), control(nullptr) {
		}

		WeakPtr(const SharedPtr<T>& shared) noexcept
			: ptr(shared.ptr), control(shared.control) {
			if (control != nullptr) {
				++control->weakCount;
			}
		}

		WeakPtr(const WeakPtr& other) noexcept
			: ptr(other.ptr), control(other.control) {
			if (control != nullptr) {
				++control->weakCount;
			}
		}

		WeakPtr(WeakPtr&& other) noexcept
			: ptr(other.ptr), control(other.control) {
			other.ptr = nullptr;
			other.control = nullptr;
		}

		~WeakPtr() {
			releaseCurrent();
		}

		WeakPtr& operator=(const WeakPtr& other) noexcept {
			if (this == &other) {
				return *this;
			}

			releaseCurrent();

			ptr = other.ptr;
			control = other.control;

			if (control != nullptr) {
				++control->weakCount;
			}

			return *this;
		}

		WeakPtr& operator=(WeakPtr&& other) noexcept {
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

		WeakPtr& operator=(const SharedPtr<T>& shared) noexcept {
			releaseCurrent();

			ptr = shared.ptr;
			control = shared.control;

			if (control != nullptr) {
				++control->weakCount;
			}

			return *this;
		}

		std::size_t use_count() const noexcept {
			if (control == nullptr) {
				return 0;
			}

			return control->sharedCount;
		}

		bool expired() const noexcept {
			return use_count() == 0;
		}

		SharedPtr<T> lock() const noexcept {
			if (expired()) {
				return SharedPtr<T>();
			}

			return SharedPtr<T>(ptr, control);
		}

		void reset() noexcept {
			releaseCurrent();
		}

		void swap(WeakPtr& other) noexcept {
			using std::swap;

			swap(ptr, other.ptr);
			swap(control, other.control);
		}
};