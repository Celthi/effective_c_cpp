static Void_t*
malloc_check(size_t sz, const Void_t *caller)
{
    Void_t *victim;

    (void)mutex_lock(&main_arena.mutex);
    victim = (top_check() >= 0) ? _int_malloc(&main_arena, sz+1) : NULL;
    (void)mutex_unlock(&main_arena.mutex);
    return mem2mem_check(victim, sz);
}

#define MAGICBYTE(p) ((((size_t)p >> 3) ^ ((size_t)p >> 11)) & 0xFF)

static Void_t*
internal_function
mem2mem_check(Void_t *ptr, size_t sz)
{
    mchunkptr p;
    unsigned char* m_ptr = (unsigned char*)BOUNDED_N(ptr, sz);
    size_t i;

    if (!ptr)
        return ptr;
    p = mem2chunk(ptr);
    for(i = chunksize(p) - (chunk_is_mmapped(p) ? 2*SIZE_SZ+1 : SIZE_SZ+1);
        i > sz;
        i -= 0xFF) {
        if(i-sz < 0x100) {
            m_ptr[i] = (unsigned char)(i-sz);
            break;
        }
        m_ptr[i] = 0xFF;
    }
    m_ptr[sz] = MAGICBYTE(p);
    return (Void_t*)m_ptr;
}


static void
free_check(Void_t* mem, const Void_t *caller)
{
    mchunkptr p;

    if(!mem) return;
    (void)mutex_lock(&main_arena.mutex);
    p = mem2chunk_check(mem);
    if(!p) {
        (void)mutex_unlock(&main_arena.mutex);

        malloc_printerr(check_action, "free(): invalid pointer", mem);
        return;
    }
    if (chunk_is_mmapped(p)) {
        (void)mutex_unlock(&main_arena.mutex);
        munmap_chunk(p);
        return;
    }
    _int_free(&main_arena, mem);
    (void)mutex_unlock(&main_arena.mutex);
}

static mchunkptr
internal_function
mem2chunk_check(Void_t* mem)
{
    mchunkptr p;
    INTERNAL_SIZE_T sz, c;
    unsigned char magic;

    if(!aligned_OK(mem)) return NULL;
    p = mem2chunk(mem);
    if (!chunk_is_mmapped(p)) {
        /* Must be a chunk in conventional heap memory. */
        int contig = contiguous(&main_arena);
        sz = chunksize(p);
        if((contig &&
           ((char*)p<mp_.sbrk_base ||
           ((char*)p+sz)>=(mp_.sbrk_base+main_arena.system_mem))) ||
           sz<MINSIZE || sz&MALLOC_ALIGN_MASK || !inuse(p) ||
           ( !prev_inuse(p) && (p->prev_size&MALLOC_ALIGN_MASK ||
           (contig && (char*)prev_chunk(p)<mp_.sbrk_base) ||
           next_chunk(prev_chunk(p))!=p) ))
                return NULL;
        magic = MAGICBYTE(p);
        for(sz += SIZE_SZ-1; (c = ((unsigned char*)p)[sz]) != magic; sz -= c) {
            if(c<=0 || sz<(c+2*SIZE_SZ)) return NULL;
        }
        ((unsigned char*)p)[sz] ^= 0xFF;
    } else {
        unsigned long offset, page_mask = malloc_getpagesize-1;

        /* mmap()ed chunks have MALLOC_ALIGNMENT or higher
           power-of-two alignment relative to the beginning of a
           page.  Check this first. */
        offset = (unsigned long)mem & page_mask;
        if((offset!=MALLOC_ALIGNMENT && offset!=0 && offset!=0x10 &&
            offset!=0x20 && offset!=0x40 && offset!=0x80 &&
            offset!=0x100 && offset!=0x200 && offset!=0x400 &&
            offset!=0x800 && offset!=0x1000 && offset<0x2000) ||
           !chunk_is_mmapped(p) || (p->size & PREV_INUSE) ||
           ((((unsigned long)p-p->prev_size) & page_mask) != 0 ) ||
           ((sz=chunksize(p)), ((p->prev_size+sz)&page_mask) != 0 ))
                  return NULL;
        magic = MAGICBYTE(p);
        for(sz-=1; (c = ((unsigned char*)p)[sz]) != magic; sz-=c) {
            if(c<=0 || sz<(c+2*SIZE_SZ)) return NULL;
        }
        ((unsigned char*)p)[sz] ^= 0xFF;
    }
    return p;
}
