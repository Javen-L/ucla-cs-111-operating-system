
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "ext2_fs.h"
#include <fcntl.h>
#include <sys/stat.h>

const int BOOTOFFSET = 1024;
const int SUPEROFFSET = 2048;

int fd;
int blockSize;
struct ext2_inode* inode;
int* s_indir;
int* d_indir;
int* t_indir;
int blockOffset = 12;

void indirect(int inum) {
	int i, j, k;
	//single indirect
	if(inode->i_block[12] == 0) return;
	s_indir = (int*)malloc(blockSize);
	if(pread(fd, s_indir, blockSize, inode->i_block[12] * blockSize) < 0) {
		fprintf(stderr, "Error on reading file, in process reading indirect.\n");
		exit(2);
	}
	for(i = 0; i < blockSize/sizeof(int); i++) {
		if(s_indir[i] == 0) continue;
		printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 1, blockOffset, inode->i_block[12], s_indir[i]);
		blockOffset++;
	}
	//double indirect
	if(inode->i_block[13] == 0) return;
	blockOffset--;
	d_indir = (int*)malloc(blockSize);
	if(pread(fd, d_indir, blockSize, inode->i_block[13] * blockSize) < 0) {
		fprintf(stderr, "Error on reading file, in process reading double indirect.\n");
		exit(2);
	}
	int flag = 0;
	int tempOffset;
	for(i = 0; i < blockSize/sizeof(int); i++) {
		if(d_indir[i] == 0) continue;
		if(flag == 0) {
   	             blockOffset = blockSize / 4 + blockOffset;
                     tempOffset = blockSize / 4 + 12;
                     flag = 1;
       //                     printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 2, tempOffset, inode->i_block[13], d_indir[i]);   
                }

		printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 2, tempOffset, inode->i_block[13], d_indir[i]);
		if(pread(fd, s_indir, blockSize, d_indir[i] * blockSize) < 0) {
			fprintf(stderr, "Error on reading file, in process reading single indirect.\n");
			exit(2);
		};
		for(j = 0; j < blockSize/sizeof(int); j++) {	
			if(s_indir[j] == 0) continue;
			printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 1, blockOffset, d_indir[i], s_indir[j]);
			blockOffset++;
		}
	}
	//trible indirect
	if(inode->i_block[14] == 0) return;
	blockOffset--;
	t_indir = (int*)malloc(blockSize);
	if(pread(fd, t_indir, blockSize, inode->i_block[14]*blockSize) < 0) {
		fprintf(stderr, "Error on reading file, in process reading trible indirect.\n");
		exit(2);
	}
	flag = 0;
	for(i = 0; i < blockSize/sizeof(int); i++) {
		if(t_indir[i] == 0) continue;
		if(flag == 0) {
                	flag = 1;
                        int temp = blockSize * (blockSize / 4) / 4;
                        blockOffset += temp;
			tempOffset = temp + 12 + blockSize / 4;	
                }
		printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 3, tempOffset, inode->i_block[14], t_indir[i]);
		if(pread(fd, d_indir, blockSize, t_indir[i] * blockSize) < 0) {
			fprintf(stderr, "Error on reading file, in process reading double indirect.\n");
			exit(2);
		}
		for(j = 0; j < blockSize/sizeof(int); j++) {
			if(d_indir[j] == 0) continue;
			printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 2, tempOffset, t_indir[i], d_indir[j]);
			if(pread(fd, s_indir, blockSize, d_indir[j] * blockSize) < 0) {
				fprintf(stderr, "Error on reading file, in process reading single indirect.\n");
				exit(2);
			}
			for(k = 0; k < blockSize/sizeof(int); k++) {
				if(s_indir[k] == 0) continue;
				printf("INDIRECT,%d,%d,%d,%d,%d\n", inum, 1, blockOffset, d_indir[j], s_indir[k]);
				blockOffset++;
			}
		}
	}
}

int main(int argc, char** argv) {
	int i, j, k, l, m, n;
	int bread, gOffset;
	struct ext2_super_block* superblock = (struct ext2_super_block*)malloc(sizeof(struct ext2_super_block));
	if(argc == 2) {
		fd = open(argv[1],O_RDONLY);
		if(fd < 0) {
			fprintf(stderr, "Can't read the file, check the file name.\n");
			exit(1);
		}
	} else {
		fprintf(stderr, "Need only provide the file name.\n");
		exit(1);
	}
	bread = pread(fd, superblock, BOOTOFFSET, BOOTOFFSET);
	if(bread < 0) {
		fprintf(stderr, "Error on reading file, in process reading superblock.\n");
		exit(2);
	}
	 blockSize = EXT2_MIN_BLOCK_SIZE << superblock->s_log_block_size;
	//print superlock
	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", superblock->s_blocks_count, superblock->s_inodes_count,
		blockSize, superblock->s_inode_size, superblock->s_blocks_per_group, superblock->s_inodes_per_group, 
		superblock->s_first_ino);
	

	int groupNum = 1 + (superblock->s_blocks_count - 1) / superblock->s_blocks_per_group;
	struct ext2_group_desc* group = (struct ext2_group_desc*)malloc(sizeof(struct ext2_group_desc));
	unsigned char* bitmap = (unsigned char*)malloc(blockSize);
	

	for(i = 0; i < groupNum; i++) {
		gOffset = SUPEROFFSET + i * blockSize * superblock->s_blocks_per_group;
		bread = pread(fd, group, sizeof(struct ext2_group_desc), gOffset);
		if(bread < 0) {
			fprintf(stderr, "Error on reading file, in process reading group.\n");
			exit(2);
		}
		//print group
		printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", i, superblock->s_blocks_count / groupNum, 
			superblock->s_inodes_count / groupNum, group->bg_free_blocks_count, 
			group->bg_free_inodes_count, group->bg_block_bitmap, group->bg_inode_bitmap,
			group->bg_inode_table);

		memset(bitmap, '\0', strlen(bitmap));
		bread = pread(fd, bitmap, blockSize, (BOOTOFFSET + (group->bg_block_bitmap - 1) * blockSize));
		if(bread < 0) {
			fprintf(stderr, "Error on reading file, in process reading bitmap.\n");
			exit(2);
		}
		//print bfree
		for(j = 0; j < blockSize; j++) {
			int val = (int)bitmap[j];
			if(val == 0) {
				for(k = 0; k < 8; k++) {
					printf("BFREE,%d\n", j * 8 + k + 1);
				}
			} else {
				for(k = 0; k < 8; k++) {
					if((val & 0x1) == 0) {
						printf("BFREE,%d\n", j * 8 + k + 1);
					}
					val >>= 1;
				}
			}
		}

		memset(bitmap, '\0', strlen(bitmap));
		bread = pread(fd, bitmap, blockSize, (BOOTOFFSET + (group->bg_inode_bitmap - 1) * blockSize));
		if(bread < 0) {
			fprintf(stderr, "Error on reading file, in process reading bitmap for inode.\n");
			exit(2);
		}
		//print ifree
		for(j = 0; j < blockSize; j++) {
			int val = (int)bitmap[j];
			if(val == 0) {
				for(k = 0; k < 8; k++) {
					printf("IFREE,%d\n", j * 8 + k + 1);
				}
			} else {
				for(k = 0; k < 8; k++) {
					if((val & 0x1) == 0) {
						printf("IFREE,%d\n", j * 8 + k + 1);
					}
					val >>= 1;	
				}
			}
		}	
		
		inode = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
		struct ext2_dir_entry* dir = (struct ext2_dir_entry*)malloc(sizeof(struct ext2_dir_entry));
		for(j = 1; j <= (superblock->s_inodes_count - group->bg_free_inodes_count); j++) {
			int val = (int)bitmap[j];
		//	printf("++%d,%d,%d\n", j, val, (superblock->s_inodes_count - group->bg_free_inodes_count));
			if(val != 0) {
				for(k = 0; k < 8; k++) {
					if((val & 0x1)!=0) {
						bread = pread(fd, inode, 128, (BOOTOFFSET + (group->bg_inode_table - 1) * blockSize + 128 * (j-1)));
						if(bread < 0) {
							fprintf(stderr, "Error on reading, in process reading inode.\n");
							exit(2);
						}
						int mode = inode->i_mode;
						int link_count = inode->i_links_count;
						//if(j == 18) {
						//	printf("++%d,%d++\n", mode, link_count);
							
						//}
						//printf("%d\n", j);
						if(mode != 0 & link_count != 0) {
							char type;
							if(S_ISREG(mode)) type = 'f';
							else if(S_ISDIR(mode)) type = 'd';
							else if(S_ISLNK(mode)) type = 's';
							else type = '?';
							time_t last_change = inode->i_ctime;
							struct tm last_change_tm;
							gmtime_r(&last_change, &last_change_tm);
							
							time_t last_modify = inode->i_mtime;
							struct tm last_modify_tm;
							gmtime_r(&last_modify, &last_modify_tm);

							time_t last_access = inode->i_atime;
							struct tm last_access_tm;
							gmtime_r(&last_access, &last_access_tm);

							//print INODE
							printf("INODE,%d,%c,%o,%d,%d,%d,%02d/%02d/%02d %02d:%02d:%02d,%02d/%02d/%02d %02d:%02d:%02d,%02d/%02d/%02d %02d:%02d:%02d,%li,%i",j,type,mode & 0xFFF,inode->i_uid,inode->i_gid,link_count,last_change_tm.tm_mon + 1,last_change_tm.tm_mday,last_change_tm.tm_year % 100, last_change_tm.tm_hour,last_change_tm.tm_min,last_change_tm.tm_sec,last_modify_tm.tm_mon + 1,last_modify_tm.tm_mday,last_modify_tm.tm_year % 100, last_modify_tm.tm_hour,last_modify_tm.tm_min,last_modify_tm.tm_sec,last_access_tm.tm_mon + 1,last_access_tm.tm_mday,last_access_tm.tm_year % 100, last_access_tm.tm_hour,last_access_tm.tm_min,last_access_tm.tm_sec,inode->i_size,inode->i_blocks);
							if(type == 'd' || type == 'f') {
								printf(",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", inode->i_block[0], inode->i_block[1], inode->i_block[2], inode->i_block[3], inode->i_block[4], inode->i_block[5], inode->i_block[6], inode->i_block[7], inode->i_block[8], inode->i_block[9], inode->i_block[10], inode->i_block[11], inode->i_block[12], inode->i_block[13], inode->i_block[14]);
							} else {
								printf(",%d\n", inode->i_block[0]);
							}
							if(type == 'd') {
								int dirOffset = 0;
								for(m = 0; m < 15; m++) {
									if(inode->i_block[m] != 0) {
										while(1) {
											if(pread(fd, dir, 264, (BOOTOFFSET + (inode->i_block[m] - 1) * blockSize + dirOffset)) < 0) {
	fprintf(stderr, "Error on reading file, in process reading dir entry.\n");
	exit(2);
}	

if(dir->inode != 0) {
	char name[255];
	strcpy(name, dir->name);
	//print diren
	printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",j,dirOffset,dir->inode,dir->rec_len,dir->name_len,name);
	dirOffset += dir->rec_len;
	if(dirOffset >= inode->i_size) {
		dirOffset = 0;
		break;
	}	
}
else {
	dirOffset = 0;
	break;
}										}
									}
								}
							}
							else {
								indirect(j);
							}
						}
					}
					val >>= 1;
					if(val != 0) j++;
				}
			}
		}
		
	}
	

}
