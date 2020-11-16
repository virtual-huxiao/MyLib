#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <io.h>
#include <cassert>

namespace ecllipse{
	template <typename Tchar>
	class basic_foperator;
	//别名
	using foperator = basic_foperator<char>;
	using wfoperator = basic_foperator<wchar_t>;
}

template <typename Tchar>
class ecllipse::basic_foperator{
	//兼容wchar_t和char类型
	using Tstring       = std::basic_string<Tchar>;
	using Tios          = std::basic_ios<Tchar,std::char_traits<Tchar>>;
	using Tistream      = std::basic_istream<Tchar,std::char_traits<Tchar>>;
	using Tostream      = std::basic_ostream<Tchar, std::char_traits<Tchar>>;
	using Tfstream      = std::basic_fstream<Tchar, std::char_traits<Tchar>>;
	using Tstringstream = std::basic_stringstream<Tchar, 
												std::char_traits<Tchar>, std::allocator<Tchar>>;
		
	//私有成员
	enum {	//判断文件状态的值
		is_write    = -1,
		is_no_state = 0,
		is_read     = 1
	};

	const Tchar* filename_;
	Tfstream* file_ptr_ = nullptr;
	int file_flag_ = is_no_state;	//判断文件是读还是写或是无状态


public:
	//构造函数
	basic_foperator() = delete;
	basic_foperator(const Tchar* filename):filename_(filename){}
	basic_foperator(const Tstring & filename):basic_foperator(filename.c_str()){}

	//析构函数
	~basic_foperator(){
		assert((file_flag_ == is_no_state));//还存有其他操作未结束!
		if(file_ptr_!=nullptr){
			delete file_ptr_;
			file_ptr_ = nullptr;
		}
	}

//读写函数
	//读写异常判断

	//读函数==================================================================
	Tstring read_all() const {
		//当前文件存在其他的操作!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		Tfstream f{ filename_ , std::ios::in};
		auto rdbuf = f.rdbuf();
		Tstringstream os;
		os << rdbuf;
		return os.str();
	}

	//开启读文件的权限
	void begin_read(){
		//当前文件存在其他的操作!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		file_ptr_ = new Tfstream{ filename_ , std::ios::in};
		file_flag_ = is_read;
	}

	//结束并处理读文件的内存
	void end_read() {
		//当前文件存在其他的操作或文件对象为空!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		delete file_ptr_;
		file_ptr_ = nullptr;
		file_flag_ = is_no_state;
	}

	//读取一行(只能使用在begin_read和end_read之间)
	Tistream& read_line(Tchar* s,std::streamsize count,
		Tchar delim = Tios::widen('\n')){
		//当前操作不符合使用规则!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return file_ptr_->getline(s,count,delim);
	}
	Tistream& read_line(Tstring& ret){
		//当前操作不符合使用规则!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return getline(*file_ptr_, ret);
	}

	//读一个字符(只能使用在begin_read和end_read之间)
	Tistream& read(Tchar& c){
		//当前操作不符合使用规则!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return file_ptr_->get(c);
	}



	//写函数==================================================================
	void write_all(const Tstring & str) const{
		this->write_all(str.c_str(), str.size());
	}
	void write_all(const Tchar* str, size_t count) const{
		//当前文件存在其他的操作!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		Tfstream f{ filename_ , std::ios::out|std::ios::app };
		f.write(str, count);
	}

	//开启写文件的权限
	void begin_write(){
		//当前文件存在其他的操作!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		file_ptr_ = new  Tfstream{ filename_ ,std::ios::out|std::ios::app};
		file_flag_ = is_write;
	}
	
	//关闭写权限并处理写文件的内存
	void end_write(){
		//当前文件存在其他非写操作!
		assert((file_flag_ == is_write) && (file_ptr_ != nullptr));
		delete file_ptr_;
		file_ptr_ = nullptr;
		file_flag_ = is_no_state;
	}

	//写入文件(只能在begin_write和end_write中使用)
	Tostream& write(const Tchar* s, std::streamsize count){
		//当前文件存在其他非写操作!
		assert((file_flag_ == is_write) && (file_ptr_ != nullptr));
		return file_ptr_->write(s, count);
	}
	Tostream& write(const Tstring& str){
		return this->write(str.c_str(), str.size());
	}

	//写入文件并自动换行(只能在begin_write和end_write中使用)
	Tostream& write_line(const Tchar* s, std::streamsize count,
		Tchar LF = std::endl){
		this->write(s, count);
		return this->write(LF, sizeof(LF));
	}
	Tostream& write_line(const Tstring& str, Tchar LF = std::endl){
		return this->write_line(str.c_str(), str.size(),LF);
	}

	//其他的文件处理===========================================================
	//清除文件内容
	bool clearn() const{
		//当前文件存在其他的操作!"
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		Tfstream f{ filename_,std::ios::out | std::ios::trunc };
		return true;
	}
	bool is_exist()const {
		if(sizeof(Tchar) == 1){
			return _access(filename_, 0) == 0;
		}else if(sizeof(Tchar) == 2){	//sizeof(wchar_t) == 2
			return _waccess(filename_, 0) == 0;
		}else{
			assert(false);//出现不应该出现的字符类型!
		}
	}

};