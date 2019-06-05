#ifndef CLICK_THREADOFFLOADSYNC_HH
#define CLICK_THREADOFFLOADSYNC_HH
#include <click/element.hh>
#include <click/sync.hh>
#include <click/tcpanno.hh>
#include "threadoffload.hh"
CLICK_DECLS

class ThreadOffloadSync : public Element
{
public:
    ThreadOffloadSync();
    ~ThreadOffloadSync();

    const char *class_name() const { return "ThreadOffloadSync"; }
    const char *port_count() const { return "1/1"; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

    Packet *simple_action(Packet *p);
private:
    uint64_t total_diff;
    uint64_t sum_count;
};

CLICK_ENDDECLS
#endif
