/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMERQUEUE_H
#define _LINUX_TIMERQUEUE_H

#include <linux/rbtree_augmented.h>
#include <linux/ktime.h>


struct timerqueue_node {
	struct rb_node node;
	ktime_t expires;
};

/**
 * stuct timerqueue_head - timerqueue base
 * @head:    rbtree root
 * @augment: If not NULL, contains augmentation callbacks to use when
 *           modifying timerqueue rbtree.
 */
struct timerqueue_head {
	struct rb_root_cached rb_root;
	const struct rb_augment_callbacks *augment;
};


extern bool timerqueue_add(struct timerqueue_head *head,
			   struct timerqueue_node *node);
extern bool timerqueue_del(struct timerqueue_head *head,
			   struct timerqueue_node *node);
extern struct timerqueue_node *timerqueue_iterate_next(
						struct timerqueue_node *node);

/**
 * timerqueue_getnext - Returns the timer with the earliest expiration time
 *
 * @head: head of timerqueue
 *
 * Returns a pointer to the timer node that has the earliest expiration time.
 */
static inline
struct timerqueue_node *timerqueue_getnext(struct timerqueue_head *head)
{
	struct rb_node *leftmost = rb_first_cached(&head->rb_root);

	return rb_entry_safe(leftmost, struct timerqueue_node, node);
}

static inline void timerqueue_init(struct timerqueue_node *node)
{
	RB_CLEAR_NODE(&node->node);
}

static inline bool timerqueue_node_queued(struct timerqueue_node *node)
{
	return !RB_EMPTY_NODE(&node->node);
}

static inline bool timerqueue_node_expires(struct timerqueue_node *node)
{
	return node->expires;
}

static inline
void timerqueue_init_head_augmented(struct timerqueue_head *head,
				    const struct rb_augment_callbacks *augment)
{
	head->rb_root = RB_ROOT_CACHED;
	head->augment = augment;
}

static inline void timerqueue_init_head(struct timerqueue_head *head)
{
	timerqueue_init_head_augmented(head, NULL);
}

static inline
struct timerqueue_node *timerqueue_getroot(const struct timerqueue_head *head)
{
	struct rb_node *rbnode = head->rb_root.rb_root.rb_node;

	if (!rbnode)
		return NULL;

	return rb_entry(rbnode, struct timerqueue_node, node);
}
#endif /* _LINUX_TIMERQUEUE_H */
