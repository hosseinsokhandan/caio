// Copyright 2023 Vahid Mardani
/*
 * This file is part of caio.
 *  caio is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  caio is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with caio. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <stddef.h>

#include "caio.h"


#undef GARR_TYPE
#define GARR_TYPE caiotask
#include "generic_array.h"
#include "generic_array.c"


static size_t callstack_size = 0;
static struct caiotask_array _tasks;


#include "caio.h"


int
caio_init(size_t maxtasks, size_t callstacksize) {
    if (caiotask_array_init(&_tasks, maxtasks)) {
        return -1;
    }

    callstack_size = callstacksize;
    return 0;
}


void
caio_deinit() {
    caiotask_array_deinit(&_tasks);
}


int
caio_task_new(caiocoro coro, void *state) {
    int index;
    struct caiotask *task = malloc(sizeof(struct caiotask));
    if (task == NULL) {
        return -1;
    }
    task->running_coros = 0;

    /* Initialize callstack */
    if (caiocall_array_init(&task->callstack, callstack_size)) {
        free(task);
        return -1;
    }

    /* Register task */
    index = caiotask_array_append(&_tasks, task);
    if (index == -1) {
        caiocall_array_deinit(&task->callstack);
        free(task);
        return -1;
    }
    task->index = index;

    if (caio_call_new(task, coro, state)) {
        caiocall_array_deinit(&task->callstack);
        free(task);
        return -1;
    }

    return 0;
}


int
caio_call_new(struct caiotask *task, caiocoro coro, void *state) {
    struct caiocall *call = malloc(sizeof(struct caiocall));
    if (call == NULL) {
        caiocall_array_deinit(&task->callstack);
        free(task);
        return -1;
    }

    call->coro = coro;
    call->state = state;
    return caiocall_array_append(&task->callstack, call);
}


int
caio_task_step(struct caiotask *task) {
    // TODO: Implement
}


int
caio_forever() {
    // int taskindex;
    // struct caiotask *task = NULL;

    // for (taskindex = 0; taskindex < _taskscount; taskindex++) {
    //     task = _tasks[taskindex];
    //     caio_task_step(task);
    // }

    return 0;
}
