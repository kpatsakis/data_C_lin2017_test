 */
BOOL BuildPngList (PTSTR pstrPathName, TCHAR **ppFileList, int *pFileCount,
     int *pFileIndex)
{
    static TCHAR              szImgPathName [MAX_PATH];
    static TCHAR              szImgFileName [MAX_PATH];
    static TCHAR              szImgFindName [MAX_PATH];
    WIN32_FIND_DATA           finddata;
    HANDLE                    hFind;
    static TCHAR              szTmp [MAX_PATH];
    BOOL                      bOk;
    int                       i, ii;
    int                       j, jj;
    /* free previous file-list */
    if (*ppFileList != NULL)
    {
        free (*ppFileList);
        *ppFileList = NULL;
    }
    /* extract foldername, filename and search-name */
    strcpy (szImgPathName, pstrPathName);
    strcpy (szImgFileName, strrchr (pstrPathName, '\\') + 1);
    strcpy (szImgFindName, szImgPathName);
    *(strrchr (szImgFindName, '\\') + 1) = '\0';
    strcat (szImgFindName, "*.png");
    /* first cycle: count number of files in directory for memory allocation */
    *pFileCount = 0;
    hFind = FindFirstFile(szImgFindName, &finddata);
    bOk = (hFind != (HANDLE) -1);
    while (bOk)
    {
        *pFileCount += 1;
        bOk = FindNextFile(hFind, &finddata);
    }
    FindClose(hFind);
    /* allocation memory for file-list */
    *ppFileList = (TCHAR *) malloc (*pFileCount * MAX_PATH);
    /* second cycle: read directory and store filenames in file-list */
    hFind = FindFirstFile(szImgFindName, &finddata);
    bOk = (hFind != (HANDLE) -1);
    i = 0;
    ii = 0;
    while (bOk)
    {
        strcpy (*ppFileList + ii, szImgPathName);
        strcpy (strrchr(*ppFileList + ii, '\\') + 1, finddata.cFileName);
        if (strcmp(pstrPathName, *ppFileList + ii) == 0)
            *pFileIndex = i;
        ii += MAX_PATH;
        i++;
        bOk = FindNextFile(hFind, &finddata);
    }
    FindClose(hFind);
    /* finally we must sort the file-list */
    for (i = 0; i < *pFileCount - 1; i++)
    {
        ii = i * MAX_PATH;
        for (j = i+1; j < *pFileCount; j++)
        {
            jj = j * MAX_PATH;
            if (strcmp (*ppFileList + ii, *ppFileList + jj) > 0)
            {
                strcpy (szTmp, *ppFileList + jj);
                strcpy (*ppFileList + jj, *ppFileList + ii);
                strcpy (*ppFileList + ii, szTmp);
                /* check if this was the current image that we moved */
                if (*pFileIndex == i)
                    *pFileIndex = j;
                else
                    if (*pFileIndex == j)
                        *pFileIndex = i;
            }
        }
    }
    return TRUE;
}
