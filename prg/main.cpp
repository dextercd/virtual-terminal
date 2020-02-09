#include <iostream>
#include <iterator>
#include <thread>

#include <cstring>
#include <cstdlib>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <virtual_terminal.hpp>

int main()
{
	try {
		auto pseudo_terminal_master = open_pseudo_terminal();

		if(grantpt(pseudo_terminal_master.native_handle()) != 0)
			throw 2;

		if(unlockpt(pseudo_terminal_master.native_handle()) != 0)
			throw 3;

		char slave_name[100];
		if(ptsname_r(pseudo_terminal_master.native_handle(), slave_name, std::size(slave_name)))
			throw 4;

		std::cout << slave_name << '\n';

		const auto pseudo_terminal_slave = open(slave_name, O_RDWR);

		const auto fork_result = fork();

		if(fork_result == -1)
			throw 5;

		if(fork_result == 0) {
			// child shouldn't access master
			pseudo_terminal_master = master_pt_handle{nullptr};

			// connect slave to stdin/stdout/stderr
			if(dup2(pseudo_terminal_slave, 0) == -1) throw 0;
			if(dup2(pseudo_terminal_slave, 1) == -1) throw 1;
			if(dup2(pseudo_terminal_slave, 2) == -1) throw 2;

			// set size
			auto size = winsize{50, 50};
			ioctl(pseudo_terminal_slave, TIOCSWINSZ, &size);

			execl("/bin/echo", "/bin/echo", "Hello world", (char*)nullptr);

			exit(1);
		}

		// parent
		const auto child_pid = fork_result;

		close(pseudo_terminal_slave);

		std::this_thread::sleep_for(std::chrono::seconds{1});
		char rb[1];
		while(read(pseudo_terminal_master.native_handle(), rb, 1) > 0)
			std::cout << rb[0];


	} catch(...) {
		std::cerr << "Error.";
	}
}
