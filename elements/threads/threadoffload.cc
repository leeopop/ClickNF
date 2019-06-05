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
#include "threadoffload.hh"
#include <click/glue.hh>
#include <click/args.hh>
#include <click/task.hh>
#include <click/error.hh>
#include <click/tcpanno.hh>
CLICK_DECLS

ThreadOffload::ThreadOffload()
{
}

ThreadOffload::~ThreadOffload()
{
}

int ThreadOffload::configure(Vector<String> &conf, ErrorHandler *errh)
{
    assert(sizeof(ThreadOffload::Annotation) <= TCP_OFFLOAD_ANNO_SIZE);
    if (Args(conf, this, errh)
            //.read_p("INTERVAL", _interval)
            //.read_p("INCREASING", _increasing)
            .complete() < 0)
        return -1;
    return 0;
}

void ThreadOffload::push(int port, Packet *p)
{
    p->kill();
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(multithread)
EXPORT_ELEMENT(ThreadOffload)
