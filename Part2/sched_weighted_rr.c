/*
 * SCHED_WEIGHTED_RR scheduling class. Implements a round robin scheduler with weight
 * priority mechanism.
 */

/*
 * Update the current task's runtime statistics. Skip current tasks that
 * are not in our scheduling class.
 Reference : https://myao0730.blogspot.tw/2016/12/linux.html
 */
static void update_curr_weighted_rr(struct rq *rq) /*rq = run queue */
{
	struct task_struct *curr = rq->curr;
	u64 delta_exec;

	if (!task_has_weighted_rr_policy(curr))	/* curr task is not in SCHED_WEIGHTED_RR */
		return;

	delta_exec = rq->clock - curr->se.exec_start;
	if (unlikely((s64)delta_exec < 0))
		delta_exec = 0;

	schedstat_set(curr->se.exec_max, max(curr->se.exec_max, delta_exec));

	curr->se.sum_exec_runtime += delta_exec;
	curr->se.exec_start = rq->clock;
	cpuacct_charge(curr, delta_exec);
}

/*
 * Adding/removing a task to/from a priority array:
 */
static void enqueue_task_weighted_rr(struct rq *rq, struct task_struct *p, int wakeup, bool b)
{ /* add task to the tail */
	// not yet implemented
	/* list_add_tail(struct list_head *new task to be added, struct list_head *head)
		Insert new entry (prev) before the specified head : new entry <-> head
	*/
	list_add_tail(&(p->weighted_rr_list_item), &((rq->weighted_rr).queue));
	rq->weighted_rr.nr_running++; /* increment the number of processes that are in the run queue now */
	// ...
}

static void dequeue_task_weighted_rr(struct rq *rq, struct task_struct *p, int sleep)
{
	// first update the task's runtime statistics
	update_curr_weighted_rr(rq);
	// not yet implemented
	/* list_del(struct list_head *entry) ; entry = the pointer to the node address which we want to delete
		Delete the task, that has just been inserted in p, from run queue 
	*/
	list_del(&(p->weighted_rr_list_item));
	rq->weighted_rr.nr_running--; /* decrement the number of processes that are in the run queue now */
	// ...
}

/*
 * Put task to the end of the run list without the overhead of dequeue
 * followed by enqueue.
 */
static void requeue_task_weighted_rr(struct rq *rq, struct task_struct *p)
{
	/* list_move_tail(struct list_head *list entry to move, struct list_head *head that will follow the entry)
		Delete the task, that has just been inserted in p, from 1 list & add as another's tail
	*/
	list_move_tail(&p->weighted_rr_list_item, &rq->weighted_rr.queue);
	/* weighted_rr_list_item = the list item which will be inserted into the run queue of weighted_rr */
}

/*
 * current process is relinquishing control of the CPU
 */
static void
yield_task_weighted_rr(struct rq *rq) /* from running state to waiting state */
{
	// not yet implemented
	/* rq->curr = the current task in run queue
	Use list_move_tail() to put the current task (rq->curr) to the end of the run list
	and rq->weighted_rr.queue will follow the current task.
	*/
	list_move_tail(&rq->curr, &rq->weighted_rr.queue);
	// ...
}

/*
 * Preempt the current task with a newly woken task if needed:
 * int wakeflags added to match function signature of other schedulers
 */
static void check_preempt_curr_weighted_rr(struct rq *rq, struct task_struct *p, int wakeflags)
{
}

/*
 * select the next task to run
 */
static struct task_struct *pick_next_task_weighted_rr(struct rq *rq) /* from waiting state to running state */
{	/* select the next task that is supposed to run, while put_prev_task is called before the currently executing task is replaced with another one */
	struct task_struct *next;
	struct list_head *queue;
	struct weighted_rr_rq *weighted_rr_rq;
	
	
	// not yet implemented
	weighted_rr_rq = &(rq->weighted_rr); /* to assign the run queue for your weighted rr to the weighted_rr_rq */
	queue = &(weighted_rr_rq->queue); /* the actual run queue of your weighted rr scheduler */
	
	if(weighted_rr_rq->nr_running == 0) return NULL /* the queue is empty & nothing to pick */
	else {
		/*list_first_entry(list head to take element from, type of struct is embedded in, member/the name of list_head within struct) */
		next = list_first_entry(queue, struct task_struct, weighted_rr_list_item);
	}
	// ...
	
	/* you need to return the selected task IN WEIGHTED_RR.queue here */
	return next;
}

static void put_prev_task_weighted_rr(struct rq *rq, struct task_struct *p)
{
	update_curr_weighted_rr(rq);
	p->se.exec_start = 0;
}

#ifdef CONFIG_SMP
/*
 * Load-balancing iterator. Note: while the runqueue stays locked
 * during the whole iteration, the current task might be
 * dequeued so the iterator has to be dequeue-safe. Here we
 * achieve that by always pre-iterating before returning
 * the current task:
 */
static struct task_struct *load_balance_start_weighted_rr(void *arg)
{	
	struct rq *rq = arg;
	struct list_head *head, *curr;
	struct task_struct *p;

	head = &rq->weighted_rr.queue;
	curr = head->prev;

	p = list_entry(curr, struct task_struct, weighted_rr_list_item);

	curr = curr->prev;

	rq->weighted_rr.weighted_rr_load_balance_head = head;
	rq->weighted_rr.weighted_rr_load_balance_curr = curr;

	return p;
}

static struct task_struct *load_balance_next_weighted_rr(void *arg)
{
	struct rq *rq = arg;
	struct list_head *curr;
	struct task_struct *p;

	curr = rq->weighted_rr.weighted_rr_load_balance_curr;

	p = list_entry(curr, struct task_struct, weighted_rr_list_item);
	curr = curr->prev;
	rq->weighted_rr.weighted_rr_load_balance_curr = curr;

	return p;
}

static unsigned long
load_balance_weighted_rr(struct rq *this_rq, int this_cpu, struct rq *busiest,
		unsigned long max_load_move,
		struct sched_domain *sd, enum cpu_idle_type idle,
		int *all_pinned, int *this_best_prio)
{
	struct rq_iterator weighted_rr_rq_iterator;

	weighted_rr_rq_iterator.start = load_balance_start_weighted_rr;
	weighted_rr_rq_iterator.next = load_balance_next_weighted_rr;
	/* pass 'busiest' rq argument into
	 * load_balance_[start|next]_weighted_rr iterators
	 */
	weighted_rr_rq_iterator.arg = busiest;

	return balance_tasks(this_rq, this_cpu, busiest, max_load_move, sd,
			     idle, all_pinned, this_best_prio, &weighted_rr_rq_iterator);
}

static int
move_one_task_weighted_rr(struct rq *this_rq, int this_cpu, struct rq *busiest,
		 struct sched_domain *sd, enum cpu_idle_type idle)
{
	struct rq_iterator weighted_rr_rq_iterator;

	weighted_rr_rq_iterator.start = load_balance_start_weighted_rr;
	weighted_rr_rq_iterator.next = load_balance_next_weighted_rr;
	weighted_rr_rq_iterator.arg = busiest;

	return iter_move_one_task(this_rq, this_cpu, busiest, sd, idle,
				  &weighted_rr_rq_iterator);
}
#endif

/*
 * task_tick_weighted_rr is invoked on each scheduler timer tick.
 */
static void task_tick_weighted_rr(struct rq *rq, struct task_struct *p,int queued)
{	/* task_tick is called by periodic scheduler each time it is activated */
	struct task_struct *curr;
	struct weighted_rr_rq *weighted_rr_rq;
	
	// first update the task's runtime statistics
	update_curr_weighted_rr(rq);

	// not yet implemented
	/* First, decrement the task_time_slice value of the task p; task_time_slice records the consumption of time slice of task p */
	p->task_time_slice--; 
	if(p->task_time_slice) return;
	else if(p->task_time_slice == 0) { /* once task_time_slice value of the task p = 0 */

		/* reset the task_time_slice of task p */
		p->task_time_slice = p->weighted_time_slice; /* how much time should be supplied when reset task_time_slice is accoding to the weighted_time_slice */
		
		/* requeue to the end of queue if we are the only element on the queue */
		if(p->weighted_rr_list_item.prev != p->weighted_rr_list_item.next) {
			set_tsk_need_resched(p);
			requeue_task_weighted_rr(rq, p); /*yield/requeue the task p */	
		}
	}

	// ...
	 
	return;	
}

/*
 * scheduling policy has changed -- update the current task's scheduling
 * statistics
 */
static void set_curr_task_weighted_rr(struct rq *rq)
{
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock;
}

/*
 * We switched to the sched_weighted_rr class.
 */
static void switched_to_weighted_rr(struct rq *rq, struct task_struct *p,
			     int running)
{
	/*
	 * Kick off the schedule if running, otherwise just see
	 * if we can still preempt the current task.
	 */
	if (running)
		resched_task(rq->curr);
	else
		check_preempt_curr(rq, p, 0);
}

static int
select_task_rq_weighted_rr(struct rq *rq, struct task_struct *p, int sd_flag, int flags)
{
	if (sd_flag != SD_BALANCE_WAKE)
		return smp_processor_id();

	return task_cpu(p);
}

const struct sched_class weighted_rr_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_weighted_rr,
	.dequeue_task		= dequeue_task_weighted_rr,
	.yield_task		= yield_task_weighted_rr,

	.check_preempt_curr	= check_preempt_curr_weighted_rr,

	.pick_next_task		= pick_next_task_weighted_rr,
	.put_prev_task		= put_prev_task_weighted_rr,

#ifdef CONFIG_SMP
	.load_balance		= load_balance_weighted_rr,
	.move_one_task		= move_one_task_weighted_rr,
#endif

	.switched_to  = switched_to_weighted_rr,
	.select_task_rq = select_task_rq_weighted_rr,

	.set_curr_task          = set_curr_task_weighted_rr,
	.task_tick		= (void *)task_tick_weighted_rr,
};
