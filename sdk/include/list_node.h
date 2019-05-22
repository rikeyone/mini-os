/*
 * Copyright (C) 2019 xiehaocheng <xiehaocheng127@163.com>
 *
 * All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifdef __cplusplus
export "C" {
#endif


#ifndef __LIST_NODE_H__
#define __LIST_NODE_H__

struct list_node {
        struct list_node *next, *prev;
} list_node_t;

#define LIST_NODE_INIT(name) { &(name), &(name) }

#define LIST_NODE(name) \
	struct list_node name = LIST_NODE_INIT(name)

#define INIT_LIST_NODE(ptr) \
	do { \
		(ptr)->next = (ptr); \
		(ptr)->prev = (ptr); \
	} while (0)

static inline void __list_node_add(struct list_node *node,
									struct list_node *prev,
									struct list_node *next)
{
        next->prev = node;
        node->next = next;
        node->prev = prev;
        prev->next = node;
}

static inline void __list_node_del(struct list_node * prev, struct list_node * next)
{
        next->prev = prev;
        prev->next = next;
}

/*
* list_node_add - add a node to the list head
* @node: node to be added
* @head: list head to add it after
*/
static inline void list_node_add(struct list_node *node, struct list_node *head)
{
        __list_node_add(node, head, head->next);
}

/*
* list_node_add_tail - add a node to the list tail
* @node: node to be added
* @head: list head to add it before
*/
static inline void list_node_add_tail(struct list_node *node, struct list_node *head)
{
        __list_node_add(node, head->prev, head);
}

/*
* list_node_del - delete a list node
* @node: list node to be deleted
*/
static inline void list_node_del(struct list_node *node)
{
        __list_node_del(node->prev, node->next);
        node->next = NULL;
        node->prev = NULL;
}

/**
* list_is_empty - test if a list is empty?
* @head: list head to test
*/
static inline int list_is_empty(const struct list_node *head)
{
        return head->next == head;
}

/*
 * list_for_each_node/list_for_each_node_prev - scan for each node of the list
 * @pos: temp variable to use
 * @head: list head to scan
 */
#define list_for_each_node(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_node_prev(pos, head) \
        for (pos = (head)->prev; pos != (head); pos = pos->prev)

/*
 * list node entry operations - scan for each entry which include list nodes
 */
#define list_node_entry(ptr, type, member) \
        (type *)( (char *)(ptr) - ((size_t) &((type *)0)->member) )

#define list_for_each_node_entry(pos, head, member) \
        for (pos = list_node_entry((head)->next, typeof(*pos), member); \
             &pos->member != (head); \
             pos = list_node_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_node_entry_prev(pos, head, member) \
        for (pos = list_node_entry((head)->prev, typeof(*pos), member); \
             &pos->member != (head); \
             pos = list_node_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_node_entry_continue(pos, head, member) \
        for (pos = list_node_entry(pos->member.next, typeof(*pos), member); \
             &pos->member != (head); \
             pos = list_node_entry(pos->member.next, typeof(*pos), member))

#endif //__LIST_NODE_H__

#ifdef __cplusplus
}
#endif
