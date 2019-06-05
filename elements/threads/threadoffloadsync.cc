/*
 * staticthreadsched.{cc,hh} -- element statically assigns tasks to threads
 * Eddie Kohler
 *
 * Copyright (c) 2004-2008 Regents of the University of California
 * Copyright (c) 2004-2014 Click authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */
#include <click/config.h>
#include "threadoffloadsync.hh"
#include <click/glue.hh>
#include <click/args.hh>
#include <click/task.hh>
#include <click/error.hh>
#include <click/tcpanno.hh>
#include <pthread.h>
#include <rte_cycles.h>
CLICK_DECLS

ThreadOffloadSync::ThreadOffloadSync()
{
}

ThreadOffloadSync::~ThreadOffloadSync()
{
}

int ThreadOffloadSync::configure(Vector<String> &conf, ErrorHandler *errh)
{
    return 0;
}

Packet* ThreadOffloadSync::simple_action(Packet *p)
{
    ThreadOffload::Annotation* anno = ThreadOffload::get_anno(p);
    if(TCP_HAS_OFFLOAD_ANNO(p) == 1) {
        DO_MICROBENCH_WITH_INTERVAL(100000);
        while(anno->state == 0);
    }
    return p;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(multithread)
EXPORT_ELEMENT(ThreadOffloadSync)
