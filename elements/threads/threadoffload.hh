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
    const char *port_count() const { return "0-/0"; }
    const char *processing() const { return PUSH; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;

    // TODO: currently, noop
    void push(int port, Packet *p);

private:
    pthread_t _worker_thread;
    int _core_id;
    struct rte_ring* job_queue;
    void* worker();
};

CLICK_ENDDECLS
#endif
