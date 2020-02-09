#ifndef PT_VIRTUAL_TERMINAL_HPP
#define PT_VIRTUAL_TERMINAL_HPP

#include <stdexcept>
#include <string>

#include <cstdlib>

#include <fcntl.h>

struct pseudo_terminal_error : std::runtime_error {
	using std::runtime_error::runtime_error;
};

// Owning handle to the master side of a pseudoterminal
class master_pt_handle {
	friend master_pt_handle open_pseudo_terminal();

	static constexpr int no_handle = -1;
	int underlying_fd;

	explicit master_pt_handle(int fd)
		: underlying_fd{fd}
	{
	}

	void close()
	{
		if(underlying_fd != no_handle)
			::close(underlying_fd);
	}

public:
	explicit master_pt_handle(std::nullptr_t)
		: underlying_fd{no_handle}
	{
	}

	master_pt_handle(master_pt_handle&& other)
		: underlying_fd{other.underlying_fd}
	{
		other.underlying_fd = no_handle;
	}

	master_pt_handle& operator=(master_pt_handle&& other)
	{
		close();

		underlying_fd = other.underlying_fd;
		other.underlying_fd = no_handle;

		return *this;
	}

	int native_handle() { return underlying_fd; }

	~master_pt_handle()
	{
		close();
	}
};

class slave_pt_handle {
	friend slave_pt_handle open_slave(master_pt_handle&);

	static constexpr int no_handle = -1;
	int underlying_fd;

	explicit slave_pt_handle(int fd)
		: underlying_fd{fd}
	{
	}

	void close()
	{
		if(underlying_fd != no_handle)
			::close(underlying_fd);
	}

public:
	explicit slave_pt_handle(std::nullptr_t)
		: underlying_fd{no_handle}
	{
	}

	slave_pt_handle(slave_pt_handle&& other)
		: underlying_fd{other.underlying_fd}
	{
		other.underlying_fd = no_handle;
	}

	slave_pt_handle& operator=(slave_pt_handle&& other)
	{
		close();

		underlying_fd = other.underlying_fd;
		other.underlying_fd = no_handle;

		return *this;
	}

	int native_handle() { return underlying_fd; }

	~slave_pt_handle()
	{
		close();
	}
};

master_pt_handle open_pseudo_terminal()
{
	const auto pt_master = posix_openpt(O_RDWR);
	if(pt_master < 0)
		throw pseudo_terminal_error{"bad pt master handle."};

	return master_pt_handle{pt_master};
}

slave_pt_handle open_slave(master_pt_handle& master)
{
	auto master_native_handle = master.native_handle();

	if(grantpt(master_native_handle) != 0)
		throw pseudo_terminal_error{"grantpt"};

	if(unlockpt(master_native_handle) != 0)
		throw pseudo_terminal_error{"unlockpt"};

	char slave_name[256];
	if(ptsname_r(master_native_handle, slave_name, std::size(slave_name)))
		throw pseudo_terminal_error{"open slave"};

	const auto slave_native_handle = open(slave_name, O_RDWR);

	return slave_pt_handle{slave_native_handle};
}

#endif // header guard
