/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include "threads/mmu.h"
#include "string.h"
#include "userprog/process.h"


#include "lib/kernel/hash.h"

#define PAGE_SIZE 4096
#define OFFSET_BITS 12

unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED);

/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void
vm_init (void) {
	vm_anon_init ();
	vm_file_init ();
#ifdef EFILESYS  /* For project 4 */
	pagecache_init ();
#endif
	register_inspect_intr ();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type (struct page *page) {
	int ty = VM_TYPE (page->operations->type);
	switch (ty) {
		case VM_UNINIT:
			return VM_TYPE (page->uninit.type);
		default:
			return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim (void);
static bool vm_do_claim_page (struct page *page);
static struct frame *vm_evict_frame (void);

/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page (spt, upage) == NULL) {
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */
		struct page *new_page = malloc(sizeof(struct page));

		

		bool (*initializer)(struct page *, enum vm_type, void *) = NULL;

		switch(VM_TYPE(type)){
			case VM_ANON:
				initializer = anon_initializer;
				break;
			default:
				break;
		}

		uninit_new(new_page, upage, init, type, aux, initializer);

		new_page->writable = writable;

		
		/* TODO: Insert the page into the spt. */
		spt_insert_page(spt, new_page);
		return true;
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL. */
struct page *
spt_find_page (struct supplemental_page_table *spt UNUSED, void *va UNUSED) {
	/* TODO: Fill this function. */

	//printf("PAGE FIND IN SPT : %p\n", va);
	//printf("SPT H_TABLE ELEM COUNT : %d\n",spt->h_table.elem_cnt );

	struct hash_elem *e;
	struct page find_page;
	find_page.va = va;

	e = hash_find(&spt->h_table, &find_page.elem);

	// struct hash_elem i;
	// hash_first (&i, spt);
	// while (hash_next (&i)) {
	// 	struct page *p = hash_entry (hash_cur (&i), struct page, elem);
	// 	printf("IN HASH : %p\n", p->va);
	// }



	return e != NULL ? hash_entry (e, struct page, elem) : NULL;

}

/* Insert PAGE into spt with validation. */
bool
spt_insert_page (struct supplemental_page_table *spt UNUSED,
		struct page *page UNUSED) {
	int succ = false;
	/* TODO: Fill this function. */
	//printf("PAGE INSERT IN SPT %p\n", page->va);


	if(hash_insert(spt, &page->elem) == NULL) succ = true;

	//succ = true;

	return succ;
}

void
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	//hash_delete(spt, &page->elem);
	vm_dealloc_page (page);
	return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	 /* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame (void) {
	struct frame *victim UNUSED = vm_get_victim ();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/

/*사용자 풀에서 palloc_get_page를 호출하여 새로운 물리 페이지를 얻는다. 
	사용자 풀에서 페이지를 성공적으로 얻었을 때, 프레임도 할당하고, 멤버들을 초기화한 후 반환한다. 
	vm_get_frame을 구현한 후에는 모든 사용자 공간 페이지(PALLOC_USER)를 이 함수를 통해 할당해야 한다. 
	페이지 할당 실패 시 스왑 아웃을 처리할 필요는 없으며, 해당 경우에는 PANIC("todo")로 표시해두면 된다.*/

static struct frame *
vm_get_frame (void) {
	struct frame *frame = NULL;
	/* TODO: Fill this function. */

	frame = malloc(sizeof(struct frame));
	//printf("VM_GET_FRAME :: CUR : %s\n", thread_current()->name);
	frame->kva = palloc_get_page(PAL_USER);
	if(frame->kva == NULL) PANIC("TODO");
	frame->page = NULL;


	ASSERT (frame != NULL);
	ASSERT (frame->page == NULL);

	return frame;
}

/* Growing the stack. */
static void
vm_stack_growth (void *addr UNUSED) {
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp (struct page *page UNUSED) {
}

/* Return true on success */
bool
vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED,
		bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */

	//printf("SEARCH FOR ADDR %p\n", addr);
	//printf("SEARCH FOR PAGE %p\n", pg_round_down(addr));


	page = spt_find_page(spt, pg_round_down(addr));

	if(page == NULL) exit(-1);

	return vm_do_claim_page (page);
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. */
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function */

	// page = malloc(sizeof(page));
	// page->va = va;

	//printf("CLAIM TO FIND %p\n", va);

	page = spt_find_page(&thread_current()->spt, va);

	ASSERT(page != NULL);

	return vm_do_claim_page (page);
}

/* Claim the PAGE and set up the mmu. */
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame ();

	/* Set links */
	frame->page = page;
	page->frame = frame;

	//printf("VM_DO_CLAIM_PAGE : page va : %p\n", page->va);
	//printf("VM_DO_CLAIM_PAGE : frame pa : %p\n", frame->kva);

	/* TODO: Insert page table entry to map page's VA to frame's PA. */

	if(!pml4_set_page(thread_current()->pml4, page->va, frame->kva, page->writable)){
		//printf("VM.C :: VM_CLAIM_PAGE : PML4 SET FAILED\n");
		return false;
	}

	//printf("VM_DO_CLAIM_PAGE :: SET PAGE HAS COMPLETED\n");



	return swap_in (page, frame->kva);
}

/* Initialize new supplemental page table */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	//printf("TRY HASH_INIT\n");
	hash_init(&spt->h_table, page_hash, hash_less_addr, NULL);
	spt->pml4 = thread_current()->pml4;
	//printf("HASH_INIT\n");
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst UNUSED,
		struct supplemental_page_table *src UNUSED) {
	// 순회를 먼저 진행하면서 하나하나 복사하고, 이중에 초기화되지 않은 페이지가 있는 경우 할당 및 즉시 클레임...

	// struct hash_elem i;
	// hash_first (&i, src);
	// while (hash_next (&i)) {
	// 	struct page *p = hash_entry (hash_cur (&i), struct page, elem);
	// 	struct page *copied_p = malloc(sizeof(struct page));

	// 	copied_p->anon = p->anon;
	// 	copied_p->file = p->file;
	// 	copied_p->frame = p->frame;
	// 	copied_p->operations = p->operations;
	// 	copied_p->uninit = p->uninit;
	// 	copied_p->va = p->va;
	// 	copied_p->writable = p->writable;
	// 	hash_insert(dst, &copied_p->elem);

		
	// 	if(p->frame != NULL){
	// 		if(!pml4_set_page(src->pml4, copied_p->va, copied_p->frame->kva, copied_p->writable)){
	// 		//printf("VM.C :: VM_CLAIM_PAGE : PML4 SET FAILED\n");
	// 		return false;
	// 		}	
	// 	}
	// }

	// printf("supplemental_page_table_copy complete\n");
	return true;
}

/* Free the resource hold by the supplemental page table */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
	//if(spt) hash_destroy(spt->h_table, destruction_supplemntal_page_table);
	//printf("HASH TRY DESTROY\n");
	if(spt == NULL) printf("SPT IS NULL\n");

	// struct hash_elem i;
	// hash_first (&i, spt);
	// while (hash_next (&i)) {
	// 	struct page *p = hash_entry (hash_cur (&i), struct page, elem);
	// 	destroy(p);
	// 	free(p);
	// }
	//printf("HASH DESTROY\n");
}

bool *hash_less_addr(const struct hash_elem *a,
		const struct hash_elem *b,
		void *aux)
{
	const struct page *data_a = hash_entry(a, struct page, elem);
	const struct page *data_b = hash_entry(b, struct page, elem);

	return (data_a->va < data_b->va);
}

void *destruction_supplemntal_page_table(struct hash_elem *hash_elem, void *aux)
{
	struct page *data = hash_entry(hash_elem, struct page, elem);
	free(data);
}

unsigned
page_hash (const struct hash_elem *p_, void *aux UNUSED) {
	const struct page *p = hash_entry (p_, struct page, elem);
	return hash_bytes (&p->va, sizeof p->va);
}
