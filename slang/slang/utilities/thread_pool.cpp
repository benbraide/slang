#include "thread_pool.h"

slang::utilities::thread_pool::thread_pool()
	: min_(0), max_(0), free_count_(0), thread_count_(0), destruct_(false){}

slang::utilities::thread_pool::thread_pool(size_type min_size, size_type max_size)
	: min_(min_size), max_(max_size), free_count_(0), thread_count_(0), destruct_(false){}

slang::utilities::thread_pool::~thread_pool(){
	lock_.lock();
	{//Stop processing
		destruct_ = true;
		min_ = max_ = 0;
		condition_.notify_all();
	}

	lock_.unlock();
	for (auto i = 0; !threads_.empty() && i < (SLANG_THREAD_POOL_POLL_TIMEOUT / SLANG_THREAD_POOL_POLL_SLEEP); ++i)
		std::this_thread::sleep_for(std::chrono::milliseconds(SLANG_THREAD_POOL_POLL_SLEEP));//Poll for threads termination
}

void slang::utilities::thread_pool::stop(){
	update_sizes(0, 0);
}

void slang::utilities::thread_pool::update_sizes(size_type min_size, size_type max_size){
	guard_type guard(lock_);
	min_ = max_ = 0;
	condition_.notify_all();
}

bool slang::utilities::thread_pool::add(task_type task, option options){
	guard_type guard(lock_);
	if (destruct_ || max_ == 0u)
		return false;

	if (options == option::dedicated || (options == option::parallel && thread_count_ >= max_))
		return add_persistent_(task);

	if (free_count_ > 0u || thread_count_ >= max_){//Wake one or more threads
		tasks_.push_back(task);
		condition_.notify_all();
	}
	else{//Add new thread
		do_run_(task, &thread_pool::run_);
		++thread_count_;
	}

	return true;
}

slang::utilities::thread_pool::size_type slang::utilities::thread_pool::min_size() const{
	return min_;
}

slang::utilities::thread_pool::size_type slang::utilities::thread_pool::max_size() const{
	return max_;
}

bool slang::utilities::thread_pool::has_tasks() const{
	return !tasks_.empty();
}

bool slang::utilities::thread_pool::add_persistent_(task_type task){
	do_run_(task, &thread_pool::run_persistent_);
	return true;
}

void slang::utilities::thread_pool::do_run_(task_type task, run_method_type method){
	auto empty_thread = threads_.emplace(threads_.end());//Emplace an empty thread
	thread_type thread([this, empty_thread, task, method](){
		(this->*method)(empty_thread, task);
	});

	*empty_thread = std::move(thread);//Move into empty
}

void slang::utilities::thread_pool::run_(thread_iterator_type thread, task_type task){
	task();//Execute task

	unique_guard_type guard(lock_);
	while (!destruct_ && max_ > 0u){
		while (!destruct_ && !tasks_.empty()){
			task = *tasks_.begin();
			tasks_.pop_front();//Remove task from list

			condition_.notify_all();
			guard.unlock();

			task();//Execute task
			guard.lock();
		}

		if (!destruct_ && thread_count_ <= min_){//Wait for next task
			++free_count_;
			condition_.wait(guard, [this]{
				return (destruct_ || max_ == 0u || !tasks_.empty());
			});

			--free_count_;
			if (thread_count_ > min_)
				break;//End thread
		}
		else//End thread
			break;
	}

	thread->detach();
	threads_.erase(thread);
	--thread_count_;
}

void slang::utilities::thread_pool::run_persistent_(thread_iterator_type thread, task_type task){
	task();//Execute task
	lock_.lock();

	thread->detach();
	threads_.erase(thread);

	lock_.unlock();
}
