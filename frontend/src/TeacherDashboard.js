import React, { useEffect, useState } from "react";
import "./Dashboard.css";

function TeacherDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [assignments, setAssignments] = useState([]);
  const [submissions, setSubmissions] = useState([]);
  const [materials, setMaterials] = useState([]);

  // View State
  const [selectedCourse, setSelectedCourse] = useState(null); // If null, show course list. If set, show course detail.
  const [activeTab, setActiveTab] = useState('materials'); // 'materials', 'assignments', 'submissions'

  // Form States
  const [courseName, setCourseName] = useState("");
  
  // Assignment Form
  const [title, setTitle] = useState("");
  const [description, setDescription] = useState("");
  const [dueDate, setDueDate] = useState("");

  // Material Form
  const [materialTitle, setMaterialTitle] = useState("");
  const [materialContent, setMaterialContent] = useState("");
  const [materialURL, setMaterialURL] = useState("");
  const [selectedFile, setSelectedFile] = useState(null);

  // 加载数据
  const fetchData = async () => {
    try {
      const [coursesRes, assignmentsRes, subsRes, materialsRes] = await Promise.all([
        fetch(`http://localhost:18080/teacher/${username}/courses`),
        fetch(`http://localhost:18080/teacher/${username}/assignments`),
        fetch(`http://localhost:18080/teacher/${username}/submissions`),
        fetch(`http://localhost:18080/teacher/${username}/materials`)
      ]);

      const coursesData = await coursesRes.json();
      const assignmentsData = await assignmentsRes.json();
      const subsData = await subsRes.json();
      const materialsData = await materialsRes.json();

      setCourses(Array.isArray(coursesData) ? coursesData : Object.values(coursesData || {}));
      setAssignments(Array.isArray(assignmentsData) ? assignmentsData : Object.values(assignmentsData || {}));
      setSubmissions(Array.isArray(subsData) ? subsData : Object.values(subsData || {}));
      setMaterials(Array.isArray(materialsData) ? materialsData : Object.values(materialsData || {}));
    } catch (error) {
      console.error("Failed to fetch dashboard data", error);
    }
  };

  useEffect(() => {
    fetchData();
  }, [username]);

  // Actions
  const handleCreateCourse = () => {
    if (!courseName) { alert("请输入课程名称"); return; }
    fetch(`http://localhost:18080/course/create`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ name: courseName, teacher: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        setCourseName("");
        fetchData();
      })
      .catch(err => alert("创建课程失败: " + err));
  };

  const handleCreateAssignment = () => {
    if (!selectedCourse || !title || !dueDate) { alert("请填写完整作业信息"); return; }
    fetch(`http://localhost:18080/assignment/create`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        course_id: selectedCourse.id,
        title,
        description,
        due_date: dueDate,
        teacher: username
      })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        setTitle("");
        setDescription("");
        setDueDate("");
        fetchData();
      })
      .catch(err => alert("布置作业失败: " + err));
  };

  const handleCreateMaterial = () => {
    if (!selectedCourse) return;
    fetch(`http://localhost:18080/course/${selectedCourse.id}/material/create`, {
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
        setMaterialTitle("");
        setMaterialContent("");
        setMaterialURL("");
        fetchData();
      })
      .catch(err => alert("发布课件失败: " + err));
  };

  const handleUploadMaterialFile = () => {
    if (!selectedCourse) return;
    if (!selectedFile) { alert('请选择要上传的文件（例如 PDF）'); return; }

    const formData = new FormData();
    formData.append('file', selectedFile);
    formData.append('teacher', username);
    formData.append('title', materialTitle || selectedFile.name);

    fetch(`http://localhost:18080/course/${selectedCourse.id}/material/upload`, {
      method: 'POST',
      body: formData
    })
      .then(res => res.json())
      .then(() => {
        alert('上传成功');
        setSelectedFile(null);
        setMaterialTitle("");
        fetchData();
      })
      .catch(err => alert('上传失败: ' + err));
  };

  const handleGrade = (submissionId, grade, comments) => {
    fetch(`http://localhost:18080/assignment/${submissionId}/grade`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ grade, comments, teacher: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        fetchData();
      })
      .catch(err => alert("评分失败: " + err));
  };

  // --- Render Helpers ---

  // Filter data for the selected course
  const currentCourseAssignments = selectedCourse 
    ? assignments.filter(a => a.course_id === selectedCourse.id) 
    : [];
  
  const currentCourseMaterials = selectedCourse
    ? materials.filter(m => m.course_id === selectedCourse.id)
    : [];

  // Submissions need to be filtered by assignments belonging to this course
  const currentCourseSubmissions = selectedCourse
    ? submissions.filter(s => {
        const assignment = assignments.find(a => a.id === s.assignment_id);
        return assignment && assignment.course_id === selectedCourse.id;
      })
    : [];


  // 1. Course List View (Dashboard Home)
  if (!selectedCourse) {
    return (
      <div className="dashboard-container">
        <header className="dashboard-header">
          <h2>👨‍🏫 教师工作台 - {username}</h2>
        </header>

        <div className="split-view">
          <div className="left-column">
            <section>
              <h3 className="section-title">📚 我的课程</h3>
              <div className="card-grid">
                {courses.length === 0 ? (
                  <div className="empty-state">暂无课程</div>
                ) : (
                  courses.map(c => (
                    <div className="card" key={c.id}>
                      <h4>{c.name}</h4>
                      <div className="card-content">
                        <p>课程ID: <strong>{c.id}</strong></p>
                      </div>
                      <div className="card-actions">
                        <button className="btn btn-primary" onClick={() => setSelectedCourse(c)}>
                          管理此课程
                        </button>
                      </div>
                    </div>
                  ))
                )}
              </div>
            </section>
          </div>

          <div className="right-column">
            <section className="form-card">
              <h3>➕ 创建新课程</h3>
              <div className="form-group">
                <input
                  className="form-control"
                  type="text"
                  placeholder="课程名称"
                  value={courseName}
                  onChange={e => setCourseName(e.target.value)}
                />
              </div>
              <button className="btn btn-primary" onClick={handleCreateCourse}>创建课程</button>
            </section>
          </div>
        </div>
      </div>
    );
  }

  // 2. Course Detail View
  return (
    <div className="dashboard-container">
      <header className="dashboard-header">
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <button className="btn btn-back" onClick={() => setSelectedCourse(null)}>← 返回列表</button>
          <h2>正在管理: {selectedCourse.name} <small style={{fontSize: '0.6em', color: '#666'}}>(ID: {selectedCourse.id})</small></h2>
        </div>
      </header>

      <div className="tabs">
        <button 
          className={`tab-button ${activeTab === 'materials' ? 'active' : ''}`}
          onClick={() => setActiveTab('materials')}
        >
          📂 课件管理
        </button>
        <button 
          className={`tab-button ${activeTab === 'assignments' ? 'active' : ''}`}
          onClick={() => setActiveTab('assignments')}
        >
          📝 作业管理
        </button>
        <button 
          className={`tab-button ${activeTab === 'submissions' ? 'active' : ''}`}
          onClick={() => setActiveTab('submissions')}
        >
          ✍️ 批改作业
        </button>
      </div>

      {/* Tab Content: Materials */}
      {activeTab === 'materials' && (
        <div className="split-view">
          <div className="left-column">
            <section className="form-card">
              <h3>📤 发布新课件</h3>
              <div className="form-group">
                <input
                  className="form-control"
                  type="text"
                  placeholder="课件标题"
                  value={materialTitle}
                  onChange={e => setMaterialTitle(e.target.value)}
                />
              </div>
              
              <div style={{ marginBottom: '15px', borderBottom: '1px solid #eee', paddingBottom: '15px' }}>
                <label style={{ display: 'block', marginBottom: '5px', fontWeight: 'bold' }}>方式一：填写内容/链接</label>
                <div className="form-group">
                  <input
                    className="form-control"
                    type="text"
                    placeholder="资源URL (可选)"
                    value={materialURL}
                    onChange={e => setMaterialURL(e.target.value)}
                  />
                </div>
                <div className="form-group">
                  <textarea
                    className="form-control"
                    placeholder="课件内容 (可选)"
                    value={materialContent}
                    onChange={e => setMaterialContent(e.target.value)}
                    style={{ minHeight: '60px' }}
                  />
                </div>
                <button className="btn btn-sm btn-primary" onClick={handleCreateMaterial}>发布文本/链接</button>
              </div>

              <div>
                <label style={{ display: 'block', marginBottom: '5px', fontWeight: 'bold' }}>方式二：上传文件 (PDF)</label>
                <div className="file-upload-area">
                  <input type="file" accept="application/pdf" onChange={e => setSelectedFile(e.target.files[0])} />
                  <div style={{ marginTop: '10px' }}>
                    <button className="btn btn-sm btn-success" onClick={handleUploadMaterialFile}>上传并发布</button>
                  </div>
                </div>
              </div>
            </section>
          </div>

          <div className="right-column">
            <section>
              <h3 className="section-title">已发布课件</h3>
              <ul className="list-group">
                {currentCourseMaterials.length === 0 ? (
                  <li className="list-item empty-state">本课程暂无课件</li>
                ) : (
                  currentCourseMaterials.map(m => (
                    <li className="list-item" key={m.id}>
                      <div className="list-item-content">
                        <strong>{m.title}</strong>
                        <div style={{ fontSize: '0.85rem', color: '#666' }}>
                          发布于: {new Date(m.created_at).toLocaleDateString()}
                        </div>
                        {m.content && <div style={{ fontSize: '0.9rem', margin: '5px 0' }}>{m.content}</div>}
                        {m.resource_url && (
                          <div style={{ marginTop: '5px' }}>
                            <a 
                              href={m.resource_url.startsWith('http') ? m.resource_url : `http://localhost:18080${m.resource_url}`} 
                              target="_blank" 
                              rel="noreferrer"
                              className="btn btn-sm btn-secondary"
                            >
                              查看资源
                            </a>
                          </div>
                        )}
                      </div>
                    </li>
                  ))
                )}
              </ul>
            </section>
          </div>
        </div>
      )}

      {/* Tab Content: Assignments */}
      {activeTab === 'assignments' && (
        <div className="split-view">
          <div className="left-column">
            <section className="form-card">
              <h3>📝 布置新作业</h3>
              <div className="form-group">
                <input
                  className="form-control"
                  type="text"
                  placeholder="作业标题"
                  value={title}
                  onChange={e => setTitle(e.target.value)}
                />
              </div>
              <div className="form-group">
                <textarea
                  className="form-control"
                  placeholder="作业描述"
                  value={description}
                  onChange={e => setDescription(e.target.value)}
                />
              </div>
              <div className="form-group">
                <label style={{ display: 'block', marginBottom: '5px', fontSize: '0.9rem' }}>截止日期</label>
                <input
                  className="form-control"
                  type="date"
                  value={dueDate}
                  onChange={e => setDueDate(e.target.value)}
                />
              </div>
              <button className="btn btn-primary" style={{ width: '100%' }} onClick={handleCreateAssignment}>
                布置作业
              </button>
            </section>
          </div>

          <div className="right-column">
            <section>
              <h3 className="section-title">已布置作业</h3>
              <ul className="list-group">
                {currentCourseAssignments.length === 0 ? (
                  <li className="list-item empty-state">本课程暂无作业</li>
                ) : (
                  currentCourseAssignments.map(a => (
                    <li className="list-item" key={a.id}>
                      <div className="list-item-content">
                        <strong>{a.title}</strong>
                        <div style={{ fontSize: '0.85rem', color: '#666' }}>截止: {a.due_date}</div>
                        <p>{a.description}</p>
                      </div>
                    </li>
                  ))
                )}
              </ul>
            </section>
          </div>
        </div>
      )}

      {/* Tab Content: Submissions */}
      {activeTab === 'submissions' && (
        <div>
          <section>
            <h3 className="section-title">学生提交列表</h3>
            <ul className="list-group">
              {currentCourseSubmissions.length === 0 ? (
                <li className="list-item empty-state">本课程暂无学生提交</li>
              ) : (
                currentCourseSubmissions.map(s => (
                  <li className="list-item" key={s.id} style={{ flexDirection: 'column', alignItems: 'flex-start' }}>
                    <div style={{ width: '100%', marginBottom: '10px' }}>
                      <div><strong>学生: {s.student}</strong> - 作业ID: {s.assignment_id}</div>
                      <div style={{ background: '#f9f9f9', padding: '10px', borderRadius: '5px', margin: '5px 0' }}>
                        {s.content}
                      </div>
                      <div style={{ fontSize: '0.85rem' }}>
                        当前成绩: <span className={`badge ${s.grade ? 'badge-success' : 'badge-warning'}`}>{s.grade || "未评分"}</span>
                      </div>
                    </div>
                    <div style={{ display: 'flex', gap: '10px', width: '100%' }}>
                      <input
                        className="form-control"
                        style={{ flex: 1 }}
                        type="text"
                        placeholder="分数"
                        onChange={e => s.newGrade = e.target.value}
                      />
                      <input
                        className="form-control"
                        style={{ flex: 2 }}
                        type="text"
                        placeholder="评语"
                        onChange={e => s.newComments = e.target.value}
                      />
                      <button 
                        className="btn btn-sm btn-primary"
                        onClick={() => handleGrade(s.id, s.newGrade, s.newComments)}
                      >
                        评分
                      </button>
                    </div>
                  </li>
                ))
              )}
            </ul>
          </section>
        </div>
      )}
    </div>
  );
}

export default TeacherDashboard;

