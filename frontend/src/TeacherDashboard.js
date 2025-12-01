import React, { useEffect, useState } from "react";

function TeacherDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [assignments, setAssignments] = useState([]);
  const [submissions, setSubmissions] = useState([]);

  const [materials, setMaterials] = useState([]);

  const [courseName, setCourseName] = useState("");
  const [courseId, setCourseId] = useState("");
  const [title, setTitle] = useState("");
  const [description, setDescription] = useState("");
  const [dueDate, setDueDate] = useState("");

  const [materialTitle, setMaterialTitle] = useState("");
  const [materialContent, setMaterialContent] = useState("");
  const [materialURL, setMaterialURL] = useState("");

  // 加载教师的课程、作业、提交情况
  useEffect(() => {
    fetch(`http://localhost:18080/teacher/${username}/courses`)
      .then(res => res.json())
      .then(data => setCourses(Array.isArray(data) ? data : Object.values(data || {})));

    fetch(`http://localhost:18080/teacher/${username}/assignments`)
      .then(res => res.json())
      .then(data => setAssignments(Array.isArray(data) ? data : Object.values(data || {})));

    fetch(`http://localhost:18080/teacher/${username}/submissions`)
      .then(res => res.json())
      .then(data => setSubmissions(Array.isArray(data) ? data : Object.values(data || {})));

    // 加载教师发布的课件
    fetch(`http://localhost:18080/teacher/${username}/materials`)
      .then(res => res.json())
      .then(data => setMaterials(Array.isArray(data) ? data : Object.values(data || {})));
  }, [username]);

  // 创建课程
  const handleCreateCourse = () => {
    fetch(`http://localhost:18080/course/create`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ name: courseName, teacher: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        return fetch(`http://localhost:18080/teacher/${username}/courses`).then(res => res.json());
      })
      .then(data => setCourses(Array.isArray(data) ? data : Object.values(data || {})))
      .catch(err => alert("创建课程失败: " + err));
  };

  // 布置新作业
  const handleCreateAssignment = () => {
    fetch(`http://localhost:18080/assignment/create`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        course_id: parseInt(courseId),
        title,
        description,
        due_date: dueDate,
        teacher: username
      })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        return fetch(`http://localhost:18080/teacher/${username}/assignments`).then(res => res.json());
      })
      .then(data => setAssignments(Array.isArray(data) ? data : Object.values(data || {})))
      .catch(err => alert("布置作业失败: " + err));
  };

  // 发布课件
  const handleCreateMaterial = () => {
    if (!courseId) {
      alert('请输入课程ID');
      return;
    }

    fetch(`http://localhost:18080/course/${courseId}/material/create`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        teacher: username,
        title: materialTitle,
        content: materialContent,
        resource_url: materialURL
      })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        return fetch(`http://localhost:18080/teacher/${username}/materials`).then(res => res.json());
      })
      .then(data => setMaterials(Array.isArray(data) ? data : Object.values(data || {})))
      .catch(err => alert("发布课件失败: " + err));
  };

  // 提交评分（统一用 submission.id）
  const handleGrade = (submissionId, grade, comments) => {
    fetch(`http://localhost:18080/assignment/${submissionId}/grade`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ grade, comments, teacher: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        return fetch(`http://localhost:18080/teacher/${username}/submissions`).then(res => res.json());
      })
      .then(data => setSubmissions(Array.isArray(data) ? data : Object.values(data || {})))
      .catch(err => alert("评分失败: " + err));
  };

  return (
    <div>
      <h2>教师主页 - {username}</h2>

      <h3>我的课程</h3>
      <ul>
        {courses.length === 0 ? (
          <li>暂无课程</li>
        ) : (
          courses.map(c => (
            <li key={c.id}>{c.name}（课程ID: {c.id}）</li>
          ))
        )}
      </ul>

      <h3>创建新课程</h3>
      <div>
        <input
          type="text"
          placeholder="课程名称"
          value={courseName}
          onChange={e => setCourseName(e.target.value)}
        />
        <button onClick={handleCreateCourse}>创建课程</button>
      </div>

      <h3>已布置的作业</h3>
      <ul>
        {assignments.length === 0 ? (
          <li>暂无作业</li>
        ) : (
          assignments.map(a => (
            <li key={a.id}>
              作业 {a.id} - {a.title} | 截止: {a.due_date} | 描述: {a.description}
            </li>
          ))
        )}
      </ul>

      <h3>布置新作业</h3>
      <div>
        <input
          type="text"
          placeholder="课程ID"
          value={courseId}
          onChange={e => setCourseId(e.target.value)}
        />
        <input
          type="text"
          placeholder="作业标题"
          value={title}
          onChange={e => setTitle(e.target.value)}
        />
        <textarea
          placeholder="作业描述"
          value={description}
          onChange={e => setDescription(e.target.value)}
        />
        <input
          type="date"
          value={dueDate}
          onChange={e => setDueDate(e.target.value)}
        />
        <button onClick={handleCreateAssignment}>布置作业</button>
      </div>

      <h3>发布课件</h3>
      <div>
        <input
          type="text"
          placeholder="课程ID"
          value={courseId}
          onChange={e => setCourseId(e.target.value)}
        />
        <input
          type="text"
          placeholder="课件标题"
          value={materialTitle}
          onChange={e => setMaterialTitle(e.target.value)}
        />
        <input
          type="text"
          placeholder="资源URL (可选)"
          value={materialURL}
          onChange={e => setMaterialURL(e.target.value)}
        />
        <textarea
          placeholder="课件内容 (可选)"
          value={materialContent}
          onChange={e => setMaterialContent(e.target.value)}
        />
        <button onClick={handleCreateMaterial}>发布课件</button>
      </div>

      <h3>已发布课件</h3>
      <ul>
        {materials.length === 0 ? (
          <li>暂无课件</li>
        ) : (
          materials.map(m => (
            <li key={m.id}>
              课程ID: {m.course_id} | {m.title} | 发布于: {m.created_at}
              <div>{m.content}</div>
              {m.resource_url && (
                <div>
                  资源: <a href={m.resource_url} target="_blank" rel="noreferrer">{m.resource_url}</a>
                </div>
              )}
            </li>
          ))
        )}
      </ul>

      <h3>学生提交情况</h3>
      <ul>
        {submissions.length === 0 ? (
          <li>暂无提交</li>
        ) : (
          submissions.map(s => (
            <li key={s.id}>
              学生 {s.student} 提交了作业 {s.assignment_id} - {s.title}: {s.content} 
              | 成绩: {s.grade || "未评分"} | 评语: {s.comments || "暂无"}
              <div>
                <input
                  type="text"
                  placeholder="分数（数字或字母）"
                  onChange={e => s.newGrade = e.target.value}
                />
                <input
                  type="text"
                  placeholder="评语"
                  onChange={e => s.newComments = e.target.value}
                />
                <button onClick={() => handleGrade(s.id, s.newGrade, s.newComments)}>
                  提交评分
                </button>
              </div>
            </li>
          ))
        )}
      </ul>
    </div>
  );
}

export default TeacherDashboard;

