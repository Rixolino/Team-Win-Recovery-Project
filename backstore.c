/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/reboot.h>

#include "backstore.h"
#include "ddftw.h"
#include "extra-functions.h"
#include "roots.h"
#include "settings_file.h"

void
nandroid_menu()
{	
	// define constants for menu selection
#define ITEM_MENU_BACK         0
#define ITEM_BACKUP_MENU       1
#define ITEM_RESTORE_MENU      2
#define ITEM_MENU_RBOOT 	   3
	
	// build headers and items in menu
	char* nan_headers[] = { "Nandroid Menu",
							"Choose Backup or Restore: ",
							NULL };
	
	char* nan_items[] = { "<- Back To Main Menu",
						  "Backup Menu",
						  "Restore Menu",
						  "-> Reboot System",
						  NULL };
	
	char** headers = prepend_title(nan_headers);
	
	int chosen_item;
    inc_menu_loc(ITEM_MENU_BACK); // record back selection into array
	for (;;)
	{
		chosen_item = get_menu_selection(headers, nan_items, 0, 0);
		switch (chosen_item)
		{
			case ITEM_MENU_BACK:
				dec_menu_loc();
				return;
			case ITEM_BACKUP_MENU:
				nan_backup_menu(0);
				break;
			case ITEM_RESTORE_MENU:
                choose_nandroid_folder();
				break;
			case ITEM_MENU_RBOOT:
                reboot(RB_AUTOBOOT);
                break;
		}
	    if (go_home) { 		// if home was called
	        dec_menu_loc();
	        return;
	    }
	}
}

char* reboot_after_nandroid()
{
	char* tmp_set = (char*)malloc(40);
	strcpy(tmp_set, "[ ] Reboot After Completed");
	if (is_true(tw_reboot_after_flash_option) == 1) {
		tmp_set[1] = 'x';
	}
	return tmp_set;
}

#define ITEM_NAN_BACKUP		0
#define ITEM_NAN_REBOOT     1
#define ITEM_NAN_SYSTEM		2
#define ITEM_NAN_DATA      	3
#define ITEM_NAN_CACHE		4
#define ITEM_NAN_BOOT       5
#define ITEM_NAN_WIMAX      6
#define ITEM_NAN_RECOVERY   7
#define ITEM_NAN_SDEXT      8
#define ITEM_NAN_ANDSEC     9
#define ITEM_NAN_BACK       10

void
nan_backup_menu(int pIdx)
{
	tw_total = 0;
	char* nan_b_headers[] = { "Nandroid Backup",
							   "Choose Nandroid Options: ",
							   NULL	};
	
	char* nan_b_items[] = { "-> Backup Naowz!",
	                         reboot_after_nandroid(),
							 nan_img_set(ITEM_NAN_SYSTEM,0),
							 nan_img_set(ITEM_NAN_DATA,0),
							 nan_img_set(ITEM_NAN_CACHE,0),
							 nan_img_set(ITEM_NAN_BOOT,0),
							 nan_img_set(ITEM_NAN_WIMAX,0),
							 nan_img_set(ITEM_NAN_RECOVERY,0),
							 nan_img_set(ITEM_NAN_SDEXT,0),
							 nan_img_set(ITEM_NAN_ANDSEC,0),
							 "<- Back To Main Menu",
							 NULL };

	char** headers = prepend_title(nan_b_headers);
	
    inc_menu_loc(ITEM_NAN_BACK);
	for (;;)
	{
		int chosen_item = get_menu_selection(headers, nan_b_items, 0, pIdx); // get key presses
		pIdx = chosen_item; // remember last selection location
		switch (chosen_item)
		{
			case ITEM_NAN_BACKUP:
				if (tw_total > 0) // only call backup if something was checked
				{
					nandroid_back_exe();
					if (is_true(tw_reboot_after_flash_option)) {
						ui_print("\nRebooting phone.\n");
						reboot(RB_AUTOBOOT);
						return;
					} else {
	            	    dec_menu_loc();
					}
					return;
				}
				break;
			case ITEM_NAN_REBOOT:
			    if (is_true(tw_reboot_after_flash_option)) {
            		strcpy(tw_reboot_after_flash_option, "0");
            	} else {
            		strcpy(tw_reboot_after_flash_option, "1");
            	}
                write_s_file();
                break;
			case ITEM_NAN_SYSTEM:
            	if (is_true(tw_nan_system_val)) {
            		strcpy(tw_nan_system_val, "0"); // toggle's value
            		tw_total--;
            	} else {
            		strcpy(tw_nan_system_val, "1");
            		tw_total++;
            	}
                write_s_file();
                break;
			case ITEM_NAN_DATA:
            	if (is_true(tw_nan_data_val)) {
            		strcpy(tw_nan_data_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_data_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_CACHE:
            	if (is_true(tw_nan_cache_val)) {
            		strcpy(tw_nan_cache_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_cache_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_BOOT:
            	if (is_true(tw_nan_boot_val)) {
            		strcpy(tw_nan_boot_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_boot_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_WIMAX:
            	if (is_true(tw_nan_wimax_val)) {
            		strcpy(tw_nan_wimax_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_wimax_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_RECOVERY:
            	if (is_true(tw_nan_recovery_val)) {
            		strcpy(tw_nan_recovery_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_recovery_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_SDEXT:
				if (tw_nan_sdext_x != -1)
				{
	            	if (is_true(tw_nan_sdext_val)) {
	            		strcpy(tw_nan_sdext_val, "0");
	            		tw_total--;
	            	} else {
	            		strcpy(tw_nan_sdext_val, "1");
	            		tw_total++;
	           		}
	                write_s_file();
				}
				break;
			case ITEM_NAN_ANDSEC:
				if (tw_nan_andsec_x != -1)
				{
					
				}
            	if (is_true(tw_nan_andsec_val)) {
            		strcpy(tw_nan_andsec_val, "0");
            		tw_total--;
            	} else {
            		strcpy(tw_nan_andsec_val, "1");
            		tw_total++;
            	}
                write_s_file();
				break;
			case ITEM_NAN_BACK:
            	dec_menu_loc();
				return;
		}
	    if (go_home) { 
	        dec_menu_loc();
	        return;
	    }
		break;
	}
	ui_end_menu(); // end menu
    dec_menu_loc();
	nan_backup_menu(pIdx); // restart menu (to refresh it)
}

void
set_restore_files()
{
	struct stat st;
	char* tmp_file = (char*)malloc(255);
	
	// check to see if images exist, so put int menu array
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_system);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_system_x = 1;
		tw_total++;
	} else {
		tw_nan_system_x = -1;
	}
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_data);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_data_x = 1;
		tw_total++;
	} else {
		tw_nan_data_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_cache);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_cache_x = 1;
		tw_total++;
	} else {
		tw_nan_cache_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_boot);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_boot_x = 1;
		tw_total++;
	} else {
		tw_nan_boot_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_wimax);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_wimax_x = 1;
		tw_total++;
	} else {
		tw_nan_wimax_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_recovery);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_recovery_x = 1;
		tw_total++;
	} else {
		tw_nan_recovery_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_sdext);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_sdext_x = 1;
		tw_total++;
	} else {
		tw_nan_sdext_x = -1;
	}	
	strcpy(tmp_file,nan_dir);
	strcat(tmp_file,tw_nan_andsec);
	if (stat(tmp_file,&st) == 0) 
	{
		tw_nan_andsec_x = 1;
		tw_total++;
	} else {
		tw_nan_andsec_x = -1;
	}
	free(tmp_file);
}

void
nan_restore_menu(int pIdx)
{
	tw_total = 0;
	char* nan_r_headers[] = { "Nandroid Restore",
							  "Choose Nandroid Options: ",
							  NULL	};
	
	char* nan_r_items[] = { "-> Restore Naowz!",
	                        reboot_after_nandroid(),
			 	 	 	 	nan_img_set(ITEM_NAN_SYSTEM,1),
			 	 	 	 	nan_img_set(ITEM_NAN_DATA,1),
			 	 	 	 	nan_img_set(ITEM_NAN_CACHE,1),
			 	 	 	 	nan_img_set(ITEM_NAN_BOOT,1),
			 	 	 	 	nan_img_set(ITEM_NAN_WIMAX,1),
			 	 	 	 	nan_img_set(ITEM_NAN_RECOVERY,1),
			 	 	 	 	nan_img_set(ITEM_NAN_SDEXT,1),
			 	 	 	 	nan_img_set(ITEM_NAN_ANDSEC,1),
							"<- Back To Main Menu",
							 NULL };

	char** headers = prepend_title(nan_r_headers);
	
    inc_menu_loc(ITEM_NAN_BACK);
	for (;;)
	{
		int chosen_item = get_menu_selection(headers, nan_r_items, 0, pIdx);
		pIdx = chosen_item; // remember last selection location
		switch (chosen_item)
		{
		case ITEM_NAN_BACKUP:
			if (tw_total > 0) // only call restore if something was checked
			{
				nandroid_rest_exe();
				if (is_true(tw_reboot_after_flash_option)) {
					ui_print("\nRebooting phone.\n");
					reboot(RB_AUTOBOOT);
					return;
				} else {
					dec_menu_loc();
				}
				return;
			}
			break;
		case ITEM_NAN_REBOOT:
			    if (is_true(tw_reboot_after_flash_option)) {
            		strcpy(tw_reboot_after_flash_option, "0");
            	} else {
            		strcpy(tw_reboot_after_flash_option, "1");
            	}
                write_s_file();
                break;	
		case ITEM_NAN_SYSTEM:
			if (tw_nan_system_x == 0)
			{
				tw_nan_system_x = 1;
        		tw_total++;
			} else if (tw_nan_system_x == 1) {
				tw_nan_system_x = 0;
        		tw_total--;
			}
            break;
		case ITEM_NAN_DATA:
			if (tw_nan_data_x == 0)
			{
				tw_nan_data_x = 1;
        		tw_total++;
			} else if (tw_nan_data_x == 1) {
				tw_nan_data_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_CACHE:
			if (tw_nan_cache_x == 0)
			{
				tw_nan_cache_x = 1;
        		tw_total++;
			} else if (tw_nan_cache_x == 1) {
				tw_nan_cache_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_BOOT:
			if (tw_nan_boot_x == 0)
			{
				tw_nan_boot_x = 1;
        		tw_total++;
			} else if (tw_nan_boot_x == 1) {
				tw_nan_boot_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_WIMAX:
			if (tw_nan_wimax_x == 0)
			{
				tw_nan_wimax_x = 1;
        		tw_total++;
			} else if (tw_nan_wimax_x == 1) {
				tw_nan_wimax_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_RECOVERY:
			if (tw_nan_recovery_x == 0)
			{
				tw_nan_recovery_x = 1;
        		tw_total++;
			} else if (tw_nan_recovery_x == 1) {
				tw_nan_recovery_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_SDEXT:
			if (tw_nan_sdext_x == 0)
			{
				tw_nan_sdext_x = 1;
        		tw_total++;
			} else if (tw_nan_sdext_x == 1) {
				tw_nan_sdext_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_ANDSEC:
			if (tw_nan_andsec_x == 0)
			{
				tw_nan_andsec_x = 1;
        		tw_total++;
			} else if (tw_nan_andsec_x == 1) {
				tw_nan_andsec_x = 0;
        		tw_total--;
			}
			break;
		case ITEM_NAN_BACK:
        	dec_menu_loc();
			return;
		}
	    if (go_home) { 
	        dec_menu_loc();
	        return;
	    }
	    break;
	}
	ui_end_menu();
    dec_menu_loc();
    nan_restore_menu(pIdx);
}

char* 
nan_img_set(int tw_setting, int tw_backstore)
{
	int isTrue = 0;
	char* tmp_set = (char*)malloc(25);
	struct stat st;
	switch (tw_setting)
	{
		case ITEM_NAN_SYSTEM:
			strcpy(tmp_set, "[ ] system");
			if (tw_backstore)
			{
				isTrue = tw_nan_system_x;
			} else {
				isTrue = is_true(tw_nan_system_val);
			}
			break;
		case ITEM_NAN_DATA:
			strcpy(tmp_set, "[ ] data");
			if (tw_backstore)
			{
				isTrue = tw_nan_data_x;
			} else {
				isTrue = is_true(tw_nan_data_val);
			}
			break;
		case ITEM_NAN_CACHE:
			strcpy(tmp_set, "[ ] cache");
			if (tw_backstore)
			{
				isTrue = tw_nan_cache_x;
			} else {
				isTrue = is_true(tw_nan_cache_val);
			}
			break;
		case ITEM_NAN_BOOT:
			strcpy(tmp_set, "[ ] boot");
			if (tw_backstore)
			{
				isTrue = tw_nan_boot_x;
			} else {
				isTrue = is_true(tw_nan_boot_val);
			}
			break;
		case ITEM_NAN_WIMAX:
			strcpy(tmp_set, "[ ] wimax");
			if (tw_backstore)
			{
				isTrue = tw_nan_wimax_x;
			} else {
				isTrue = is_true(tw_nan_wimax_val);
			}
			break;
		case ITEM_NAN_RECOVERY:
			strcpy(tmp_set, "[ ] recovery");
			if (tw_backstore)
			{
				isTrue = tw_nan_recovery_x;
			} else {
				isTrue = is_true(tw_nan_recovery_val);
			}
			break;
		case ITEM_NAN_SDEXT:
			strcpy(tmp_set, "[ ] sd-ext");
			if (tw_backstore)
			{
				isTrue = tw_nan_sdext_x;
			} else {
				if (stat(sde.blk,&st) != 0) 
				{
					tw_nan_sdext_x = -1;
					isTrue = -1;
				} else {
					isTrue = is_true(tw_nan_sdext_val);
				}
			}
			break;
		case ITEM_NAN_ANDSEC:
			strcpy(tmp_set, "[ ] .android-secure");
			if (tw_backstore)
			{
				isTrue = tw_nan_andsec_x;
			} else {
				if (stat(ase.dev,&st) != 0) 
				{
					tw_nan_andsec_x = -1;
					isTrue = -1;
				} else {
					isTrue = is_true(tw_nan_andsec_val);
				}
			}
			break;
	}
	if (isTrue == 1)
	{
		tmp_set[1] = 'x';
		tw_total++;
	} 
	if (isTrue == -1) 
	{
		tmp_set[0] = ' ';
		tmp_set[1] = ' ';
		tmp_set[2] = ' ';
	}
	return tmp_set;
}

void 
nandroid_back_exe()
{
	char exe[255];
	char tw_image_base[100];
	char tw_image[255];
	struct stat st;
    char timestamp[14];
    struct tm * t;
    time_t seconds;
    seconds = time(0);
    t = localtime(&seconds);
    sprintf(timestamp,"%02d%02d%d%02d%02d",t->tm_mon+1,t->tm_mday,t->tm_year+1900,t->tm_hour,t->tm_min); // get timestamp for nandroid
	sprintf(tw_image_base, "%s/%s/", nandroid_folder, device_id);
	if (stat(tw_image_base,&st) != 0) {
		if(mkdir(tw_image_base,0777) == -1) {
			LOGI("=> Can not create directory: %s\n", tw_image_base);
		} else {
			LOGI("=> Created directory: %s\n", tw_image_base);
		}
	}
	strcat(tw_image_base,timestamp);
	strcat(tw_image_base,"/");
	if(mkdir(tw_image_base,0777) == -1) {
		LOGI("=> Can not create directory: %s\n", tw_image_base);
	} else {
		LOGI("=> Created directory: %s\n", tw_image_base);
	}

	ui_print("\nStarting Backup...\n\n");
	if (is_true(tw_nan_system_val)) {
		ensure_path_mounted("/system");
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_system);
		if (strcmp(sys.fst,"yaffs2") == 0)
		{
			sprintf(exe,"mkyaffs /%s %s", sys.mnt, tw_image);
		} else {
			sprintf(exe,"dd bs=2048 if=%s of=%s", sys.dev, tw_image);
		}
		LOGI("=> %s\n", exe);
		ui_print("...Backing up system partition.\n");
		ui_show_progress(1,300);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", sys.mnt);
		makeMD5(tw_image_base,tw_nan_system);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", sys.mnt);
		checkMD5(tw_image_base,tw_nan_system);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_data_val)) {
		ensure_path_mounted("/data");
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_data);
		if (strcmp(dat.fst,"yaffs2") == 0)
		{
			sprintf(exe,"mkyaffs /%s %s", dat.mnt, tw_image);
		} else {
			sprintf(exe,"dd bs=2048 if=%s of=%s", dat.dev, tw_image);
		}
		LOGI("=> %s\n", exe);
		ui_print("...Backing up data partition.\n");
		ui_show_progress(1,300);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", dat.mnt);
		makeMD5(tw_image_base,tw_nan_data);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", dat.mnt);
		checkMD5(tw_image_base,tw_nan_data);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_cache_val)) {
		ensure_path_mounted("/cache");
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_cache);
		if (strcmp(cac.fst,"yaffs2") == 0)
		{
			sprintf(exe,"mkyaffs /%s %s", cac.mnt, tw_image);
		} else {
			sprintf(exe,"dd bs=2048 if=%s of=%s", cac.dev, tw_image);
		}
		LOGI("=> %s\n", exe);
		ui_print("...Backing up cache partition.\n");
		ui_show_progress(1,100);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", cac.mnt);
		makeMD5(tw_image_base,tw_nan_cache);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", cac.mnt);
		checkMD5(tw_image_base,tw_nan_cache);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_boot_val)) {
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_boot);
		sprintf(exe,"dd bs=2048 if=%s of=%s", boo.dev, tw_image);
		LOGI("=> %s\n", exe);
		ui_print("...Backing up boot partition.\n");
		ui_show_progress(1,5);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", boo.mnt);
		makeMD5(tw_image_base,tw_nan_boot);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", boo.mnt);
		checkMD5(tw_image_base,tw_nan_boot);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_wimax_val)) {
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_wimax);
		sprintf(exe,"dd bs=2048 if=%s of=%s", wim.dev, tw_image);
		LOGI("=> %s\n", exe);
		ui_print("...Backing up wimax partition.\n");
		ui_show_progress(1,5);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", wim.mnt);
		makeMD5(tw_image_base,tw_nan_wimax);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", wim.mnt);
		checkMD5(tw_image_base,tw_nan_wimax);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_recovery_val)) {
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_recovery);
		sprintf(exe,"dd bs=2048 if=%s of=%s", rec.dev, tw_image);
		LOGI("=> %s\n", exe);
		ui_print("...Backing up recovery partition.\n");
		ui_show_progress(1,5);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", rec.mnt);
		makeMD5(tw_image_base,tw_nan_recovery);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", rec.mnt);
		checkMD5(tw_image_base,tw_nan_recovery);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_sdext_val)) {
		ensure_path_mounted("/sd-ext");
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_sdext);
		sprintf(exe,"cd %s && tar -cvzf %s ./*", sde.mnt, tw_image);
		LOGI("=> %s\n", exe);
		ui_print("...Backing up sd-ext partition.\n");
		ui_show_progress(1,30);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", sde.mnt);
		makeMD5(tw_image_base,tw_nan_sdext);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", sde.mnt);
		checkMD5(tw_image_base,tw_nan_sdext);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	if (is_true(tw_nan_andsec_val)) {
		ensure_path_mounted(ase.dev);
		strcpy(tw_image,tw_image_base);
		strcat(tw_image,tw_nan_andsec);
		sprintf(exe,"cd %s && tar -cvzf %s ./*", ase.dev, tw_image);
		LOGI("=> %s\n", exe);
		ui_print("...Backing up .android_secure.\n");
		ui_show_progress(1,30);
		__system(exe);
		ui_print("....Done.\n");
		ui_print("...Generating %s md5...\n", ase.mnt);
		makeMD5(tw_image_base,tw_nan_andsec);
		ui_print("....Done.\n");
		ui_print("...Verifying %s md5...\n", ase.mnt);
		checkMD5(tw_image_base,tw_nan_andsec);
		ui_print("...Done.\n\n");
		ui_reset_progress();
	}
	ui_print("Backup Completed.\n\n");
}

void 
nandroid_rest_exe()
{
	char exe[255];
	char* tmp_file = (char*)malloc(255);
	ui_print("\nStarting Restore...\n\n");
	if (tw_nan_system_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_system);
		ui_show_progress(1,300);
		if(checkMD5(nan_dir,tw_nan_system))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_system);
			if (strcmp(sys.fst,"yaffs2") == 0)
			{
				sprintf(exe,"/%s",sys.mnt);
				erase_volume(exe);
				ensure_path_mounted("/system");
				sprintf(exe,"unyaffs %s /%s",tmp_file,sys.mnt);
			} else {
				sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, sys.dev);
			}
			LOGI("=> %s\n", exe);
			ui_print("...Restoring system partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_data_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_data);
		ui_show_progress(1,300);
		if(checkMD5(nan_dir,tw_nan_data))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_data);
			if (strcmp(dat.fst,"yaffs2") == 0)
			{
				sprintf(exe,"/%s",dat.mnt);
				erase_volume(exe);
				ensure_path_mounted("/data");
				sprintf(exe,"unyaffs %s /%s",tmp_file,dat.mnt);
			} else {
				sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, dat.dev);
			}
			LOGI("=> %s\n", exe);
			ui_print("...Restoring data partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_cache_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_cache);
		ui_show_progress(1,100);
		if(checkMD5(nan_dir,tw_nan_cache))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_cache);
			if (strcmp(cac.fst,"yaffs2") == 0)
			{
				sprintf(exe,"/%s",cac.mnt);
				erase_volume(exe);
				ensure_path_mounted("/cache");
				sprintf(exe,"unyaffs %s /%s",tmp_file,cac.mnt);
			} else {
				sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, cac.dev);
			}
			LOGI("=> %s\n", exe);
			ui_print("...Restoring cache partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_boot_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_boot);
		ui_show_progress(1,5);
		if(checkMD5(nan_dir,tw_nan_boot))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_boot);
			sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, boo.dev);
			LOGI("=> %s\n", exe);
			ui_print("...Restoring boot partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_wimax_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_wimax);
		ui_show_progress(1,5);
		if(checkMD5(nan_dir,tw_nan_wimax))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_wimax);
			sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, wim.dev);
			LOGI("=> %s\n", exe);
			ui_print("...Restoring wimax partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_recovery_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_recovery);
		ui_show_progress(1,5);
		if(checkMD5(nan_dir,tw_nan_recovery))
		{
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_recovery);
			sprintf(exe,"dd bs=2048 if=%s of=%s", tmp_file, rec.dev);
			LOGI("=> %s\n", exe);
			ui_print("...Restoring recovery partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_sdext_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_sdext);
		ui_show_progress(1,30);
		if(checkMD5(nan_dir,tw_nan_sdext))
		{
			ensure_path_mounted(sde.mnt);
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_sdext);
			sprintf(exe,"rm -rf %s/* 2>/dev/null", sde.mnt);
			ui_print("...Wiping %s.\n",sde.mnt);
			__system(exe);
			sprintf(exe,"cd %s && tar -xv -zf %s", sde.dev, tmp_file);
			LOGI("=> %s\n", exe);
			ui_print("...Restoring sd-ext partition.\n");
			__system(exe);
			ui_print("...Done.\n\n");
		} else {
			ui_print("...Failed md5 check. Aborted.\n");
		}
		ui_reset_progress();
	}
	if (tw_nan_andsec_x == 1) {
		ui_print("...Verifying md5 hash for %s.\n",tw_nan_andsec);
		ui_show_progress(1,30);
		if(checkMD5(nan_dir,tw_nan_andsec))
		{
			ensure_path_mounted(ase.dev);
			strcpy(tmp_file,nan_dir);
			strcat(tmp_file,tw_nan_andsec);
			sprintf(exe,"rm -rf %s/* 2>/dev/null", ase.dev);
			ui_print("...Wiping %s.\n",ase.dev);
			__system(exe);
			sprintf(exe,"cd %s && tar -xv -zf %s", ase.dev, tmp_file);
			LOGI("=> %s\n", exe);
			ui_print("...Restoring .android-secure.\n");
			__system(exe);
			ui_print("...Done.\n\n");	
		} else {
			ui_print("...Failed md5 check. Aborted.\n");
		}
		ui_reset_progress();
	}
	ui_print("Restore Completed.\n\n");
	free(tmp_file);
}

static int compare_string(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void choose_nandroid_folder() 
{
    ensure_path_mounted(SDCARD_ROOT);

    char tw_dir[255];
    sprintf(tw_dir, "%s/%s/", nandroid_folder, device_id);
    const char* MENU_HEADERS[] = { "Choose a Nandroid Folder:",
    							   tw_dir,
                                   NULL };
    
    char** headers = prepend_title(MENU_HEADERS);
    
    DIR* d;
    struct dirent* de;
    d = opendir(tw_dir);
    if (d == NULL) {
        LOGE("error opening %s: %s\n", tw_dir, strerror(errno));
        ensure_path_unmounted(SDCARD_ROOT);
        return;
    }

    int d_size = 0;
    int d_alloc = 10;
    char** dirs = malloc(d_alloc * sizeof(char*));
    int z_size = 1;
    int z_alloc = 10;
    char** zips = malloc(z_alloc * sizeof(char*));
    zips[0] = strdup("../");
    
    inc_menu_loc(0);
    while ((de = readdir(d)) != NULL) {
        int name_len = strlen(de->d_name);

        if (de->d_type == DT_DIR) {
            if (name_len == 1 && de->d_name[0] == '.') continue;
            if (name_len == 2 && de->d_name[0] == '.' &&
                de->d_name[1] == '.') continue;

            if (d_size >= d_alloc) {
                d_alloc *= 2;
                dirs = realloc(dirs, d_alloc * sizeof(char*));
            }
            dirs[d_size] = malloc(name_len + 2);
            strcpy(dirs[d_size], de->d_name);
            dirs[d_size][name_len] = '/';
            dirs[d_size][name_len+1] = '\0';
            ++d_size;
        }
    }
    closedir(d);

    qsort(dirs, d_size, sizeof(char*), compare_string);
    qsort(zips, z_size, sizeof(char*), compare_string);

    if (d_size + z_size + 1 > z_alloc) {
        z_alloc = d_size + z_size + 1;
        zips = realloc(zips, z_alloc * sizeof(char*));
    }
    memcpy(zips + z_size, dirs, d_size * sizeof(char*));
    free(dirs);
    z_size += d_size;
    zips[z_size] = NULL;

    int chosen_item = 0;
    for (;;) 
    {
        chosen_item = get_menu_selection(headers, zips, 1, chosen_item);

        char* item = zips[chosen_item];
        int item_len = strlen(item);

        if (chosen_item == 0) {
            break;
        } else if (item[item_len-1] == '/') {
            strcpy(nan_dir, tw_dir);
            strcat(nan_dir, item);
            set_restore_files();
            nan_restore_menu(0);
            break;
        }
    }

    int i;
    for (i = 0; i < z_size; ++i) free(zips[i]);
    free(zips);

    ensure_path_unmounted(SDCARD_ROOT);
    dec_menu_loc();
}

int makeMD5(char* imgDir, char* imgFile)
{
	int bool = 0;
	if (ensure_path_mounted("/sdcard") != 0) {
		LOGI("=> Can not mount sdcard.\n");
	} else {
		char tmpString[255];
		sprintf(tmpString,"cd %s && md5sum %s > %s.md5",imgDir,imgFile,imgFile);
		__system(tmpString);
		bool = 1;
	}
	return bool;
}

int checkMD5(char* imgDir, char* imgFile)
{
	int bool = 0;
	if (ensure_path_mounted("/sdcard") != 0) {
		LOGI("=> Can not mount sdcard.\n");
	} else {
		FILE *fp;
		char tmpString[255];
		char tmpAnswer[10];
		sprintf(tmpString,"cd %s && md5sum -c %s.md5",imgDir,imgFile);
		fp = __popen(tmpString, "r");
		if (fp == NULL)
		{
			LOGI("=> Can not open pipe.\n");
		} else {
			fgets(tmpString, 255, fp);
			sscanf(tmpString,"%*s %s",tmpAnswer);
			if (strcmp(tmpAnswer,"OK") == 0)
			{
				bool = 1;
				ui_print("....MD5 Check: %s\n", tmpAnswer);
			} else {
				ui_print("....MD5 Error: %s\n", tmpString);
			}
		}
		__pclose(fp);
	}
	return bool;
}