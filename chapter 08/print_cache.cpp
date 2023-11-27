void PrintCacheStatus()
{
    int i;
    MEM_SIZET sz;
    MEM_SIZET totalSz = 0;

    printf("Size(KB)\t#Blocks\tTotal_Size(KB)\n");
    for (i=0, sz=64 K; i<MAX_NUM_BANDS; i++)
    {
        unsigned int listsz = 0;
        CacheBlock* block;

        block = g_cache_lists_heads[i];
        while (block)
        {
            listsz++;
            block = block->next;
        }
        printf("%ld\t\t%d\t\t%ld\n",
               sz/1024, listsz, sz*listsz/1024);
        totalSz += sz*listsz;

        sz = sz << 1;
    }
    printf("------------------------------------------\n");
    printf("\t\t\t\t%ld\n", totalSz/1024);
}
