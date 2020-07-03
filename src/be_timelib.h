void my_sleep(double duration)
{
    time_t start = time(NULL);
    printf("%f", duration);
    time_t now;
    do {
        now = time(NULL);
    } while (difftime(now, start) < duration);
}
