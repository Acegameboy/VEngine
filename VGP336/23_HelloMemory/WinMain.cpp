#include <VEngine/Inc/VEngine.h>

using namespace VEngine;
using namespace VEngine::Core;

static int sUniqueId = 0;
class Student
{
public:
	Student()
	{
		mId = ++sUniqueId;
		mMark = 60 + (rand() % 41);
		mName = "NoName_" + std::to_string(mId);
	}
	Student(const std::string& name)
		:mName(name)
	{
		mId = ++sUniqueId;
		mMark = 60 + (rand() % 41);
	}
	Student(const std::string& name, int mark)
		:mName(name)
		,mMark(mark)
	{
		mId = ++sUniqueId;
		mMark = 60 + (rand() % 41);
	}
	void Log()
	{
		LOG("Name: %s - ID: %d - Mark: %d", mName.c_str(), mId, mMark);
	}
private:
	int mId = 0;
	int mMark = 0;
	std::string mName;
};

int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	TypedAllocator studentPool = TypedAllocator<Student>("StudentPool", 100);

	std::vector<Student*> students;
	//Test NEW
	for (uint32_t i = 0; i < 70; ++i)
	{
		std::string newStudentName = "NewStudents" + std::to_string(i);
		Student* newStudent = studentPool.New(newStudentName, 75);
		newStudent->Log();
		students.push_back(newStudent);
	}

	//Test Delete
	for (uint32_t i = 0; i < 40; ++i)
	{
		Student* student = students.back();
		studentPool.Delete(students.back());
		students.pop_back();
	}

	//Test add more
	for (uint32_t i = 0; i < 40; ++i)
	{
		Student* newStudent = studentPool.New();
		newStudent->Log();
		students.push_back(newStudent);
	}

	//Cleanup
	for (Student* student : students)
	{
		student->Log();
		studentPool.Delete(student);
	}
	students.clear();

	return 0;
}