#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>

#define HZ	100

typedef struct tag_process_basic_info
{
	int i_pid;
	int i_has_exit;
	int i_exit_code;
}PROCESS_BASIC_S;

typedef struct tag_process_exec_info
{
	int i_total_running_time;
	int i_cpu_time;
	int i_io_time;
}PROCESS_EXEC_S;

typedef struct tag_process_info
{
	PROCESS_BASIC_S st_process_basic;
	PROCESS_EXEC_S  st_process_exec;
}PROCESS_S;

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char * argv[])
{
	PROCESS_S st_child_processes[] =
	{
		{{0, 0, 0}, {10, 1, 0}},
		{{0, 0, 0}, {10, 0, 1}},
		{{0, 0, 0}, {10, 1, 1}},
		{{0, 0, 0}, {10, 1, 9}},
		{{0, 0, 0}, {10, 9, 1}}
	};
	int i_processes_num;
	int i_loop;
	int i_current_finish_pid;
	int i_current_exit_code = 0;

	/* create children processes */
	i_processes_num = sizeof(st_child_processes) / sizeof(PROCESS_S);
	for (i_loop = 0; i_loop < i_processes_num; i_loop++)
	{
		st_child_processes[i_loop].st_process_basic.i_pid = fork();
		if (!st_child_processes[i_loop].st_process_basic.i_pid)
		{
			cpuio_bound(st_child_processes[i_loop].st_process_exec.i_total_running_time,
				        st_child_processes[i_loop].st_process_exec.i_cpu_time,
						st_child_processes[i_loop].st_process_exec.i_io_time);

			/* child process no need to create other children processes,
			   so return directly */
			return 0;
		}

		printf("child process[%d] [%d] created\n", i_loop, st_child_processes[i_loop].st_process_basic.i_pid);
	}

	/* wait for all processes finished */
	while ((i_current_finish_pid = wait(&i_current_exit_code)) > 0)
	{
		for (i_loop = 0; i_loop < i_processes_num; i_loop++)
		{
			/* 如果进行已经退出，则不需要再判断 */
			if (st_child_processes[i_loop].st_process_basic.i_has_exit == 1)
			{
				continue;
			}

			if (st_child_processes[i_loop].st_process_basic.i_pid == i_current_finish_pid)
			{
				st_child_processes[i_loop].st_process_basic.i_has_exit = 1;
				st_child_processes[i_loop].st_process_basic.i_exit_code = i_current_exit_code;
				printf("child process[%d] [%d] finished, exit code[%d]\n",
					   i_loop, i_current_finish_pid, i_current_exit_code);
			}
		}
	}

	printf("father process finished\n");

	return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}

