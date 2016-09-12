#include "lfs_operations.h"
/*
static int lfs_access (const char *path, int mask);

//fuse operations interface mapping
struct fuse_operations lfs_oper = {
    // .getattr    = lfs_getattr,
    // .mkdir      = lfs_mkdir,
    // .readdir    = lfs_readdir,
    // .open       = lfs_open,
    // .read       = lfs_read,
    // .write      = lfs_write,
    // .create     = lfs_create,
    .access     = lfs_access,
    // .unlink     = lfs_unlink,
    // .rmdir      = lfs_rmdir,
    // .rename     = lfs_rename
};

*/
// static int lfs_access (const char *path, int mask) {
  // printf ("++++++++++++++++++++++++++++++++++Entry: lfs_access()\n");
//   printf ("++++++++++++++++++++++++++++++++++Path = %s\n", path);
//   if (strcmp (path, lfs_info->cur_path) == 0) {
//     //get_inode_from_inode_id (lfs_info->cur_inode, 1);
//     printf ("+++++++++++++++++++++++++++++++++++++++we do not change cur inode\n");
//     return 0;
//   }
//
//   inode_t *inode = (inode_t *)malloc (sizeof (inode_t));
//   memset (inode, 0, sizeof (inode_t));
//   uint32_t ret;
//   printf ("++++++++++++before comapring, parentpath is %s\n", lfs_info->par_path);
//   //char * parent_path = get_parentpath (lfs_info->cur_path);
//   //printf ("+++++++++++++++++++++++Parent Path == %s\n", parent_path);
//   if (strcmp (lfs_info->par_path, path) == 0) {
//     printf ("+++++++++++++++++++++++++++++++++PARENT Path\n");
//     ret = dir_get_inode ("..", inode);
//     if (ret != 0) {
//         printf ("Exit: Entry doesn't exist\n");
//         return -ENOENT;
//     }
//
//     lfs_info->cur_inode = inode;
//     strcpy (lfs_info->cur_path, lfs_info->par_path);
//     printf ("++++++++++++++DEBUGGGGG: cur_path now is: %s\n", lfs_info->cur_path);
//     char *new_parpath = get_parentpath (lfs_info->cur_path);
//     strcpy (lfs_info->par_path, new_parpath);
//     printf ("+++++++++++++after comparing and .., global par_path = %s, global cur_path = %s\n", lfs_info->par_path, lfs_info->cur_path);
//     return ret;
//   }
//   else {
//     printf ("+++++++++++++++++++++++++++++++++REGULAR Path\n");
//     ret = dir_get_inode (path, inode);
//   }
//   if (ret != 0) {
//     printf ("++++++++++++++++++++++++++++++Exit: lfs_access exit. inode not found\n");
//     return -ENOENT;
//   }
//   if (inode->inode_type == DIRECTORY) {
//     lfs_info->cur_inode = inode;
//     //lfs_info->par_path = lfs_info->cur_path;
//     //lfs_info->cur_path = path;
//     strcpy (lfs_info->par_path, lfs_info->cur_path);
//     strcpy (lfs_info->cur_path, path);
//   }
//   printf ("+++++++++++++++++++++++++++lfs_access: Set new cur_inode in lfs_info\n");
//   printf ("+++++++++++++++++++++++++++lfs_create: new inode num is: %u, par_path is %s, cur_path is %s\n", lfs_info->cur_inode->inode_id, lfs_info->par_path, lfs_info->cur_path);
//   printf ("++++++++++++++++++++++++++++Exit: lfs_access exit. inode found\n");
//   return 0;
// }
