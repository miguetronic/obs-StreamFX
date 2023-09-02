// AUTOGENERATED COPYRIGHT HEADER START
// Copyright (C) 2020-2023 Michael Fabian 'Xaymar' Dirks <info@xaymar.com>
// Copyright (C) 2022 lainon <GermanAizek@yandex.ru>
// AUTOGENERATED COPYRIGHT HEADER END

#pragma once
#include "common.hpp"

#include "warning-disable.hpp"
#include <functional>
#include <list>
#include <mutex>
#include "warning-enable.hpp"

namespace streamfx::util {
	template<typename... _args>
	class event {
		std::list<std::function<void(_args...)>> _listeners;
		std::recursive_mutex                     _lock;

		std::function<void()> _cb_fill;
		std::function<void()> _cb_clear;

		public /* constructor */:
		event() : _listeners(), _lock(), _cb_fill(), _cb_clear() {}
		virtual ~event()
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			this->clear();
		}

		/* Copy Constructor */
		event(const event<_args...>&) = delete;

		/* Move Constructor */
		event(event<_args...>&& other) noexcept : event()
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			std::lock_guard<std::recursive_mutex> lgo(other._lock);

			_listeners.swap(other._listeners);
			_cb_fill.swap(other._cb_fill);
			_cb_clear.swap(other._cb_clear);
		}

		public /* operators */:
		/* Copy Operator */
		event<_args...>& operator=(const event<_args...>&) = delete;

		/* Move Operator */
		event<_args...>& operator=(event<_args...>&& other) noexcept
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			std::lock_guard<std::recursive_mutex> lgo(other._lock);

			_listeners.swap(other._listeners);
			_cb_fill.swap(other._cb_fill);
			_cb_clear.swap(other._cb_clear);

			return *this;
		}

		/** Call the event, going through all listeners in the order they were registered in.
		*/
		template<typename... _largs>
		inline void operator()(_args... args)
		{
			call<_largs...>(args...);
		}
		template<typename... _largs>
		inline void call(_args... args)
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			for (auto& l : _listeners) {
				l(args...);
			}
		}

		public /* functions: listeners */:
		/** Add a new listener to the event.
		 * @param listener A listener bound with std::bind or a std::function.
		 */
		inline void add(std::function<void(_args...)> listener)
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			if (_listeners.size() == 0) {
				if (_cb_fill) {
					_cb_fill();
				}
			}
			_listeners.push_back(listener);
		}
		inline event<_args...>& operator+=(std::function<void(_args...)> listener)
		{
			this->add(listener);
			return *this;
		}

		/** Remove an existing listener from the event.
		 * @param listener A listener bound with std::bind or a std::function.
		 */
		inline void remove(std::function<void(_args...)> listener)
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			_listeners.remove(listener);
			if (_listeners.size() == 0) {
				if (_cb_clear) {
					_cb_clear();
				}
			}
		}
		inline event<_args...>& operator-=(std::function<void(_args...)> listener)
		{
			this->remove(listener);
			return *this;
		}

		/** Check if there are any listeners for the event.
		 * @return bool `true` if there are none, otherwise `false`.
		 */
		inline bool empty()
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			return _listeners.empty();
		}
		inline operator bool()
		{
			return !this->empty();
		}

		/** Clear the list of listeners for the event.
		 */
		inline void clear()
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			_listeners.clear();
			if (_cb_clear) {
				_cb_clear();
			}
		}
		inline event<_args...>& operator=(std::nullptr_t)
		{
			clear();
			return *this;
		}

		public /* callbacks */:
		void set_listen_callback(std::function<void()> cb)
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			this->_cb_fill = cb;
		}

		void set_silence_callback(std::function<void()> cb)
		{
			std::lock_guard<std::recursive_mutex> lg(_lock);
			this->_cb_clear = cb;
		}
	};
} // namespace streamfx::util
