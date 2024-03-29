// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <iosource/PktSrc.h>

extern "C" {
#include <testimony.h>
}

#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <sys/types.h> // for u_char

namespace iosource {
namespace testimony {

class TestimonySource : public iosource::PktSrc {
public:
	TestimonySource(const std::string& path, bool is_live);
	~TestimonySource() override;

	static PktSrc* Instantiate(const std::string& path, bool is_live);

protected:
	// PktSrc interface.
	void Open() override;
	void Close() override;
	bool ExtractNextPacket(Packet* pkt) override;
	void DoneWithPacket() override;
	bool PrecompileFilter(int index, const std::string& filter) override;
	bool SetFilter(int index) override;
	void Statistics(Stats* stats) override;

private:
	void OpenLive();
	void AddPacketsToTemporaryQueue();

	::testimony td;
	::testimony_iter td_iter;
	timeval curr_timeval;
	tpacket3_hdr *curr_packet;
	std::queue<tpacket3_hdr *> packets;
	std::queue<tpacket3_hdr *> temp_packets;

	std::atomic<bool> running;
	std::mutex queue_access_mutex;
	std::thread fill_queue_thread{};

	Properties props;
	Stats stats;
};

}
}
