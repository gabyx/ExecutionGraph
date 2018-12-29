## Order Not Important & No Data Synchronization
Use a strand because we do not lock the `std::cout`
Post through `io_service`, tell it to execute on the `strand`

```cpp
void PrintNum(int x)
{
    std::cout << x << " " << std::endl;
}

boost::this_thread::sleep(boost::posix_time::milliseconds(100));
io_service->post(strand.wrap(boost::bind(&PrintNum, 1)));
io_service->post(strand.wrap(boost::bind(&PrintNum, 2)));

boost::this_thread::sleep(boost::posix_time::milliseconds(100));
io_service->post(strand.wrap(boost::bind(&PrintNum, 3)));
io_service->post(strand.wrap(boost::bind(&PrintNum, 4)));
```

Possible Output: `2 1 3 4`
No ordering guaranteed, all are serialized by means of the `strand` -> meaning **no** `PrintNum` is concurrent (but different threads can still be involved)
and the shared resources among all `PrintNum`, `std::cout`, does not need locking!

## Order Important & No Data Synchronization

Ordering is guaranteed, all are serialized by means of the `strand` -> meaning **no** `PrintNum` is concurrent (different threads can still be involved)
and the shared resources among all `PrintNum`, `std::cout`, does not need locking!
```cpp
strand.post(boost::bind(&PrintNum, 1));
strand.post(boost::bind(&PrintNum, 2));
strand.post(boost::bind(&PrintNum, 3));
strand.post(boost::bind(&PrintNum, 4));
```
Output: `1 2 3 4`

## Order Unimportant & Data Synchronization (implicitly) needed
All execute concurrently among the threads, so the access to `std::cout`, as it is a shared resource, 
needs to synchronized by a mutex for example.
```cpp
io_service->post(boost::bind(&PrintNum, 1))
io_service->post(boost::bind(&PrintNum, 2))
io_service->post(boost::bind(&PrintNum, 3))
io_service->post(boost::bind(&PrintNum, 4))
```
Possible Output: `3 1 2 4`