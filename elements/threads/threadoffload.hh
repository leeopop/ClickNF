#ifndef CLICK_THREADOFFLOAD_HH
#define CLICK_THREADOFFLOAD_HH
#include <click/element.hh>
#include <click/sync.hh>
#include <click/tcpanno.hh>
CLICK_DECLS

class ThreadOffload : public Element
{
public:
    typedef struct {
        rte_atomic16_t state;
    }Annotation;

    static inline ThreadOffload::Annotation* get_anno(Packet* p) {
        ThreadOffload::Annotation* ptr = (ThreadOffload::Annotation*)p->anno_u8(TCP_FLAGS_ANNO_OFFSET);
        return ptr;
    }

    static inline void set_anno(Packet* p, ThreadOffload::Annotation* source) {
        ThreadOffload::Annotation* ptr = (ThreadOffload::Annotation*)p->anno_u8(TCP_FLAGS_ANNO_OFFSET);
        memcpy(ptr, source, sizeof(ThreadOffload::Annotation));
    }

    ThreadOffload();
    ~ThreadOffload();

    const char *class_name() const { return "ThreadOffload"; }
    const char *port_count() const { return PORTS_1_0; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

    // TODO: currently, noop
    void push(int port, Packet *p);

private:
};

CLICK_ENDDECLS
#endif
