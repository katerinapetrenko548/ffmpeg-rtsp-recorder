#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <rtsp_url> <segment_dir>\n";
        return 1;
    }

    std::string rtsp_url = argv[1];
    std::string segment_dir = argv[2];

    std::string segmentPath =
        segment_dir + "/segment_%Y-%m-%dT%H-%M-%S.ts";

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid > 0) {
        std::cout << "FFmpeg started with PID " << pid << "\n";
        return 0;
    }

    // Child
    setsid();

    int outFd = ::open("logs/out.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    int errFd = ::open("logs/err.log", O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (outFd >= 0) dup2(outFd, STDOUT_FILENO);
    if (errFd >= 0) dup2(errFd, STDERR_FILENO);

    if (outFd > 2) close(outFd);
    if (errFd > 2) close(errFd);

    const char* ffmpeg = "/usr/bin/ffmpeg";

    std::vector<std::string> args = {
        ffmpeg,
        "-loglevel", "info",
        "-rtsp_transport", "tcp",
        "-i", rtsp_url,
        "-map", "0:v",
        "-map", "0:a?",
        "-c:v", "copy",
        "-c:a", "copy",
        "-f", "segment",
        "-segment_time", "10",
        "-reset_timestamps", "1",
        "-strftime", "1",
        segmentPath
    };

    std::vector<char*> argvv;
    for (auto& s : args)
        argvv.push_back(const_cast<char*>(s.c_str()));
    argvv.push_back(nullptr);

    execv(ffmpeg, argvv.data());

    // execv only returns on error
    perror("execv failed");
    _exit(127);
}
