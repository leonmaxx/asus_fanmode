/*
 * file.hpp
 *
 *  Created on: Nov 12, 2019
 *      Author: Leonid Maksymchuk
 */

#ifndef FILE_HPP_
#define FILE_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

class File {
public:
					File();
					File(const std::string& rsFileName, int nMode = O_RDONLY);
					~File();

	bool			isValid() const;
	bool			open(const std::string& rsFileName, int nMode = O_RDONLY);

	ssize_t			read(void* pvPtr, size_t nSize);
	ssize_t			write(const void* pvPtr, size_t nSize);

	size_t			size() const;
	int				handle() const;
	void			close();

private:
	int				m_nFd;

};

inline File::File() {
	m_nFd = -1;
}

inline File::File(const std::string& rsFileName, int nMode) {
	open(rsFileName, nMode);
}

inline File::~File() {
	close();
}

inline bool File::isValid() const {
	return m_nFd >= 0;
}

inline bool File::open(const std::string& rsFileName, int nMode) {
	m_nFd = ::open(rsFileName.c_str(), nMode);
	return isValid();
}

inline ssize_t File::read(void* pvPtr, size_t nSize) {
	return ::read(m_nFd, pvPtr, nSize);
}

inline ssize_t File::write(const void* pvPtr, size_t nSize) {
	return ::write(m_nFd, pvPtr, nSize);
}

inline size_t File::size() const {
	struct stat stStat;
	if (isValid() && !fstat(m_nFd, &stStat)) {
		return stStat.st_size;
	}
	return 0;
}

inline int File::handle() const {
	return m_nFd;
}

inline void File::close() {
	if (isValid()) {
		::close(m_nFd);
		m_nFd = -1;
	}
}

#endif
