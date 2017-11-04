#ifndef EX_ALGORITHM_H_
#define EX_ALGORITHM_H_

#include<algorithm>
#include<iterator>
#include<tuple>
#include<string>
#include<regex>
#include<sstream>
#include<set>
#include<ctime>

#define _Singleton(_class) (exAlgorithm::Singleton<_class>::getInstance())

namespace exAlgorithm {
	template<class set_type>
	set_type set_union(const set_type& s1, const set_type& s2) {
		set_type result;
		std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(),
			std::inserter(result, result.begin()));
		return result;
	}
	template<class set_type>
	set_type set_difference(const set_type& s1, const set_type& s2) {
		set_type result;
		std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
			std::inserter(result, result.begin()));
		return result;
	}
	template<class set_type, class _expand, class _orthogonalize, class _union>
	void closure(set_type &value, _expand expand, _orthogonalize isEmpty,
		_union valueUnion, bool _all_participate, set_type *first_mid = nullptr) {
		set_type *_mid_result = (!_all_participate && first_mid != nullptr) ? first_mid : &value;
		if (first_mid && !isEmpty(*first_mid)) valueUnion(value, *first_mid);
		while (true) {
			auto *_expand_result = expand(value, *_mid_result);
			if (isEmpty(*_expand_result)) {
				if (_mid_result != &value) delete _mid_result;
				return;
			}
			valueUnion(value, *_expand_result);
			if (!_all_participate) {
				if (_mid_result != &value) delete _mid_result;
				_mid_result = _expand_result;
			}
			else delete _expand_result;
		}
	}
	template<class map_set_type, class _expand>
	void map_set_closure(map_set_type &value, _expand expand, bool _all_participate, map_set_type *first_mid = nullptr) {
		auto isEmpty = [&](map_set_type &mid_result)->bool {
			bool ret = true;
			for (auto &pair : mid_result) {
				const auto &_item = value[pair.first];
				pair.second = exAlgorithm::set_difference(pair.second, _item);
				if (!pair.second.empty()) ret = false;
			}
			return ret;
		};
		auto Union = [](map_set_type &result, map_set_type &mid)->void {
			for (const auto &pair : mid)  result[pair.first].insert(pair.second.begin(), pair.second.end());
		};
		closure(value, expand, isEmpty, Union, _all_participate, first_mid);
	}

	template<typename Tuple, size_t N, class Func>
	struct tuple_traverse {
		static void traverse(const Tuple& t, Func func) {
			tuple_traverse<Tuple, N - 1, Func>::traverse(t, func);
			func(std::get<N - 1>(t));
		}
	};
	template<typename Tuple, class Func>
	struct tuple_traverse<Tuple, 1, Func> {
		static void traverse(const Tuple& t, Func func) { func(std::get<0>(t)); }
	};
	template<typename... Args, class Func>
	void for_each(const std::tuple<Args...>& t, Func func) {
		tuple_traverse<decltype(t), sizeof...(Args), Func>::traverse(t, func);
	}
	template<typename T>
	void stylized_print(std::ostream &out, const std::string &option, T& value) { out << value; }
	template<typename... Args>
	std::string string_format(const std::string &pattern, Args&&... args) {
		std::ostringstream sstr;
		std::smatch m;
		using format_pattern = enum { All, Label, Index, Option, Param, Plain, PlainChar, LeftBrace };
		auto parameter = std::forward_as_tuple(args...);
		std::regex reg("(\\{(.*?)(:(.*?))?\\})|(([^\\{]|(\\{\\{))*)"), e("\\{\\{");
		std::string::const_iterator it = pattern.begin();
		std::string::const_iterator end = pattern.end();
		while (std::regex_search(it, end, m, reg)) {
			if (m[Plain].matched)
				sstr << std::regex_replace(m[Plain].str(), e, "{");
			else {
				std::string index = m[Index].str();
				int position = std::atoi(index.c_str());
				if (position || *index.c_str() == '0') {
					int kind = 0;
					if (std::tuple_size<decltype(parameter)>::value > position)
					for_each(parameter, [&](auto &param)->void {
                        if (kind == position){
							if (!m[Option].matched) sstr << param;
                            else stylized_print(sstr, m[Param].str(), param);
                        }
						kind++;
					});
				}
			}
			it = m[All].second;
			if (it == end) break;
		}
		sstr << std::ends;
		return sstr.str();
	}
	template<typename ... Args>
	std::string cstring_format(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		char* buf = new char[size];
		snprintf(buf, size, format.c_str(), args ...);
		return std::string(buf, size - 1);
	}
	template<class T>
	class Singleton {
	public:
		static T& getInstance(){
			static T singleT;
			return singleT;
		}
	};
	class Timer {
		clock_t _start;
		int duration;
		bool _isStart;
	public:
		Timer() :duration(0),_isStart(false) {}
		void start() {
			if (!_isStart) {
				_isStart = true;
				_start = clock();
			}
		}
		void end() {
			if (_isStart) {
				duration += clock() - _start;
				_isStart = false;
			}
		}
		int getTicks() const { return duration; }
		double getDuration()const { return double(duration) / CLOCKS_PER_SEC; }
	};

}
#endif
