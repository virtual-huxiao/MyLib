#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <io.h>
#include <cassert>

namespace ecllipse{
	template <typename Tchar>
	class basic_foperator;
	//����
	using foperator = basic_foperator<char>;
	using wfoperator = basic_foperator<wchar_t>;
}

template <typename Tchar>
class ecllipse::basic_foperator{
	//����wchar_t��char����
	using Tstring       = std::basic_string<Tchar>;
	using Tios          = std::basic_ios<Tchar,std::char_traits<Tchar>>;
	using Tistream      = std::basic_istream<Tchar,std::char_traits<Tchar>>;
	using Tostream      = std::basic_ostream<Tchar, std::char_traits<Tchar>>;
	using Tfstream      = std::basic_fstream<Tchar, std::char_traits<Tchar>>;
	using Tstringstream = std::basic_stringstream<Tchar, 
												std::char_traits<Tchar>, std::allocator<Tchar>>;
		
	//˽�г�Ա
	enum {	//�ж��ļ�״̬��ֵ
		is_write    = -1,
		is_no_state = 0,
		is_read     = 1
	};

	const Tchar* filename_;
	Tfstream* file_ptr_ = nullptr;
	int file_flag_ = is_no_state;	//�ж��ļ��Ƕ�����д������״̬


public:
	//���캯��
	basic_foperator() = delete;
	basic_foperator(const Tchar* filename):filename_(filename){}
	basic_foperator(const Tstring & filename):basic_foperator(filename.c_str()){}

	//��������
	~basic_foperator(){
		assert((file_flag_ == is_no_state));//��������������δ����!
		if(file_ptr_!=nullptr){
			delete file_ptr_;
			file_ptr_ = nullptr;
		}
	}

//��д����
	//��д�쳣�ж�

	//������==================================================================
	Tstring read_all() const {
		//��ǰ�ļ����������Ĳ���!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		Tfstream f{ filename_ , std::ios::in};
		auto rdbuf = f.rdbuf();
		Tstringstream os;
		os << rdbuf;
		return os.str();
	}

	//�������ļ���Ȩ��
	void begin_read(){
		//��ǰ�ļ����������Ĳ���!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		file_ptr_ = new Tfstream{ filename_ , std::ios::in};
		file_flag_ = is_read;
	}

	//������������ļ����ڴ�
	void end_read() {
		//��ǰ�ļ����������Ĳ������ļ�����Ϊ��!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		delete file_ptr_;
		file_ptr_ = nullptr;
		file_flag_ = is_no_state;
	}

	//��ȡһ��(ֻ��ʹ����begin_read��end_read֮��)
	Tistream& read_line(Tchar* s,std::streamsize count,
		Tchar delim = Tios::widen('\n')){
		//��ǰ����������ʹ�ù���!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return file_ptr_->getline(s,count,delim);
	}
	Tistream& read_line(Tstring& ret){
		//��ǰ����������ʹ�ù���!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return getline(*file_ptr_, ret);
	}

	//��һ���ַ�(ֻ��ʹ����begin_read��end_read֮��)
	Tistream& read(Tchar& c){
		//��ǰ����������ʹ�ù���!
		assert((file_flag_ == is_read) && (file_ptr_ != nullptr));
		return file_ptr_->get(c);
	}



	//д����==================================================================
	void write_all(const Tstring & str) const{
		this->write_all(str.c_str(), str.size());
	}
	void write_all(const Tchar* str, size_t count) const{
		//��ǰ�ļ����������Ĳ���!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		Tfstream f{ filename_ , std::ios::out|std::ios::app };
		f.write(str, count);
	}

	//����д�ļ���Ȩ��
	void begin_write(){
		//��ǰ�ļ����������Ĳ���!
		assert((file_flag_ == is_no_state) && (file_ptr_ == nullptr));
		file_ptr_ = new  Tfstream{ filename_ ,std::ios::out|std::ios::app};
		file_flag_ = is_write;
	}
	
	//�ر�дȨ�޲�����д�ļ����ڴ�
	void end_write(){
		//��ǰ�ļ�����������д����!
		assert((file_flag_ == is_write) && (file_ptr_ != nullptr));
		delete file_ptr_;
		file_ptr_ = nullptr;
		file_flag_ = is_no_state;
	}

	//д���ļ�(ֻ����begin_write��end_write��ʹ��)
	Tostream& write(const Tchar* s, std::streamsize count){
		//��ǰ�ļ�����������д����!
		assert((file_flag_ == is_write) && (file_ptr_ != nullptr));
		return file_ptr_->write(s, count);
	}
	Tostream& write(const Tstring& str){
		return this->write(str.c_str(), str.size());
	}

	//д���ļ����Զ�����(ֻ����begin_write��end_write��ʹ��)
	Tostream& write_line(const Tchar* s, std::streamsize count,
		Tchar LF = std::endl){
		this->write(s, count);
		return this->write(LF, sizeof(LF));
	}
	Tostream& write_line(const Tstring& str, Tchar LF = std::endl){
		return this->write_line(str.c_str(), str.size(),LF);
	}

	//�������ļ�����===========================================================
	//����ļ�����
	bool clearn() const{
		//��ǰ�ļ����������Ĳ���!"
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
			assert(false);//���ֲ�Ӧ�ó��ֵ��ַ�����!
		}
	}

};