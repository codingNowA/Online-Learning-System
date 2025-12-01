import React, { useEffect, useState } from "react";
import "./Dashboard.css";

function StudentDashboard({ username }) {
  const [courses, setCourses] = useState([]);
  const [availableCourses, setAvailableCourses] = useState([]);
  const [submissions, setSubmissions] = useState([]);
  const [pendingAssignments, setPendingAssignments] = useState([]);
  
  // View State
  const [selectedCourse, setSelectedCourse] = useState(null); // If set, show course detail
  const [activeTab, setActiveTab] = useState('materials'); // 'materials', 'assignments'
  const [showSubmitModal, setShowSubmitModal] = useState(false);

  // Assignment Submission Form State
  const [assignmentId, setAssignmentId] = useState("");
  const [submissionTitle, setSubmissionTitle] = useState("");
  const [content, setContent] = useState("");
  const [targetAssignmentTitle, setTargetAssignmentTitle] = useState(""); // For display in modal
  
  // Data Cache for Course Details
  const [courseMaterials, setCourseMaterials] = useState({}); // { courseId: [materials] }

  // 加载已选课程、可选课程、已提交作业、未完成作业
  const fetchData = async () => {
    try {
      const [coursesRes, availableRes, subsRes, pendingRes] = await Promise.all([
        fetch(`http://localhost:18080/student/${username}/courses`),
        fetch(`http://localhost:18080/student/${username}/available_courses`),
        fetch(`http://localhost:18080/student/${username}/submissions`),
        fetch(`http://localhost:18080/student/${username}/pending_assignments`)
      ]);

      const coursesData = await coursesRes.json();
      const availableData = await availableRes.json();
      const subsData = await subsRes.json();
      const pendingData = await pendingRes.json();

      setCourses(Array.isArray(coursesData) ? coursesData : Object.values(coursesData || {}));
      setAvailableCourses(Array.isArray(availableData) ? availableData : Object.values(availableData || {}));
      setSubmissions(Array.isArray(subsData) ? subsData : Object.values(subsData || {}));
      setPendingAssignments(Array.isArray(pendingData) ? pendingData : Object.values(pendingData || {}));
    } catch (error) {
      console.error("Failed to fetch dashboard data", error);
    }
  };

  useEffect(() => {
    fetchData();
  }, [username]);

  // Fetch materials when entering a course
  const handleEnterCourse = (course) => {
    setSelectedCourse(course);
    setActiveTab('materials');
    // Fetch materials for this course if not already cached (or always fetch to be fresh)
    fetch(`http://localhost:18080/course/${course.id}/materials`)
      .then(res => res.json())
      .then(data => {
        const list = Array.isArray(data) ? data : Object.values(data || {});
        setCourseMaterials(prev => ({ ...prev, [course.id]: list }));
      })
      .catch(err => console.error('获取课件失败: ' + err));
  };

  const handleEnroll = (courseId) => {
    fetch(`http://localhost:18080/course/${courseId}/enroll`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ student: username })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        fetchData();
      })
      .catch(err => alert("选课失败: " + err));
  };

  const openSubmitModal = (assignment) => {
    setAssignmentId(assignment.id);
    setTargetAssignmentTitle(assignment.title);
    setSubmissionTitle(`${username} - ${assignment.title}`);
    setContent("");
    setShowSubmitModal(true);
  };

  const closeSubmitModal = () => {
    setShowSubmitModal(false);
    setAssignmentId("");
    setTargetAssignmentTitle("");
    setSubmissionTitle("");
    setContent("");
  };

  // 提交作业
  const handleSubmit = () => {
    if (!assignmentId || !submissionTitle || !content) {
      alert("请填写完整信息");
      return;
    }
    fetch(`http://localhost:18080/assignment/${username}/submit`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        assignment_id: parseInt(assignmentId),
        title: submissionTitle,
        content
      })
    })
      .then(res => res.text())
      .then(msg => {
        alert(msg);
        closeSubmitModal();
        fetchData();
      })
      .catch(err => alert("提交失败: " + err));
  };

  // --- Render Helpers ---

  if (!selectedCourse) {
    return (
      <div className="dashboard-container">
        <header className="dashboard-header">
          <h2>👋 欢迎回来, {username}</h2>
        </header>

        <div className="split-view">
          {/* 左侧栏：课程列表 */}
          <div className="left-column">
            <section>
              <h3 className="section-title">📚 我的课程</h3>
              <div className="card-grid">
                {courses.length === 0 ? (
                  <div className="empty-state">暂无已选课程</div>
                ) : (
                  courses.map(c => (
                    <div className="card" key={c.id} onClick={() => handleEnterCourse(c)} style={{cursor: 'pointer'}}>
                      <h4>{c.name}</h4>
                      <div className="card-content">
                        <p>教师: {c.teacher}</p>
                        <p>ID: {c.id}</p>
                      </div>
                      <div className="card-actions">
                        <button className="btn btn-sm btn-primary">进入课程</button>
                      </div>
                    </div>
                  ))
                )}
              </div>
            </section>

            <section style={{ marginTop: '40px' }}>
              <h3 className="section-title">🔍 可选课程</h3>
              <ul className="list-group">
                {availableCourses.length === 0 ? (
                  <li className="list-item empty-state">暂无新课程可选</li>
                ) : (
                  availableCourses.map(c => (
                    <li className="list-item" key={c.id}>
                      <div className="list-item-content">
                        <strong>{c.name}</strong> (ID: {c.id}) - 教师: {c.teacher}
                      </div>
                      <button className="btn btn-sm btn-success" onClick={() => handleEnroll(c.id)}>
                        选课
                      </button>
                    </li>
                  ))
                )}
              </ul>
            </section>
          </div>

          {/* 右侧栏：全局作业待办 */}
          <div className="right-column">
            <section>
              <h3 className="section-title">📝 待办作业 (全部)</h3>
              <div className="card-grid" style={{ gridTemplateColumns: '1fr' }}>
                {pendingAssignments.length === 0 ? (
                  <div className="empty-state">真棒！没有待办作业</div>
                ) : (
                  pendingAssignments.map(a => (
                    <div className="card" key={a.id} style={{ borderLeft: '4px solid var(--warning-color)' }}>
                      <h4>{a.title}</h4>
                      <div className="card-content">
                        <p><strong>截止:</strong> {a.due_date}</p>
                        <p>{a.description}</p>
                        <p><small>作业ID: {a.id}</small></p>
                      </div>
                      <div className="card-actions">
                        <button 
                          className="btn btn-sm btn-primary"
                          onClick={() => openSubmitModal(a)}
                        >
                          去提交
                        </button>
                      </div>
                    </div>
                  ))
                )}
              </div>
            </section>
            
            <section style={{ marginTop: '20px' }}>
              <h3 className="section-title">✅ 最近提交</h3>
              <ul className="list-group">
                {submissions.slice(0, 5).map(s => (
                  <li className="list-item" key={s.id}>
                    <div className="list-item-content">
                      <div><strong>作业 {s.assignment_id}</strong>: {s.title}</div>
                      <div>
                        <span className={`badge ${s.grade ? 'badge-success' : 'badge-warning'}`}>
                          {s.grade ? `成绩: ${s.grade}` : '未评分'}
                        </span>
                      </div>
                    </div>
                  </li>
                ))}
              </ul>
            </section>
          </div>
        </div>

        {/* Global Submit Modal */}
        {showSubmitModal && (
          <div className="modal-overlay">
            <div className="modal">
              <div className="modal-header">
                <h3>提交作业: {targetAssignmentTitle}</h3>
                <button className="close-button" onClick={closeSubmitModal}>×</button>
              </div>
              <div className="modal-body">
                <div className="form-group">
                  <label>作业标题</label>
                  <input
                    className="form-control"
                    type="text"
                    value={submissionTitle}
                    onChange={e => setSubmissionTitle(e.target.value)}
                  />
                </div>
                <div className="form-group">
                  <label>作业内容</label>
                  <textarea
                    className="form-control"
                    rows="5"
                    placeholder="在此输入作业内容..."
                    value={content}
                    onChange={e => setContent(e.target.value)}
                  />
                </div>
              </div>
              <div className="modal-footer">
                <button className="btn btn-secondary" onClick={closeSubmitModal}>取消</button>
                <button className="btn btn-primary" onClick={handleSubmit}>确认提交</button>
              </div>
            </div>
          </div>
        )}
      </div>
    );
  }

  // Course Detail View
  const currentMaterials = courseMaterials[selectedCourse.id] || [];
  const currentCourseAssignments = pendingAssignments.filter(a => a.course_id === selectedCourse.id);

  return (
    <div className="dashboard-container">
      <header className="dashboard-header">
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <button className="btn btn-back" onClick={() => setSelectedCourse(null)}>← 返回首页</button>
          <h2>{selectedCourse.name} <small style={{fontSize: '0.6em', color: '#666'}}>(教师: {selectedCourse.teacher})</small></h2>
        </div>
      </header>

      <div className="tabs">
        <button 
          className={`tab-button ${activeTab === 'materials' ? 'active' : ''}`}
          onClick={() => setActiveTab('materials')}
        >
          📂 课程资料
        </button>
        <button 
          className={`tab-button ${activeTab === 'assignments' ? 'active' : ''}`}
          onClick={() => setActiveTab('assignments')}
        >
          📝 待办作业
          {currentCourseAssignments.length > 0 && <span className="badge badge-warning" style={{marginLeft: '5px'}}>{currentCourseAssignments.length}</span>}
        </button>
      </div>

      {activeTab === 'materials' && (
        <section>
          <div className="card-grid" style={{ gridTemplateColumns: '1fr' }}>
            {currentMaterials.length === 0 ? (
              <div className="empty-state">该课程暂无课件</div>
            ) : (
              currentMaterials.map(m => (
                <div className="card" key={m.id}>
                  <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                    <div>
                      <h4>{m.title}</h4>
                      <p style={{ color: '#666', fontSize: '0.9rem' }}>发布于: {new Date(m.created_at).toLocaleDateString()}</p>
                      <p>{m.content}</p>
                    </div>
                    {m.resource_url && (
                      <a 
                        href={m.resource_url.startsWith('http') ? m.resource_url : `http://localhost:18080${m.resource_url}`} 
                        target="_blank" 
                        rel="noreferrer"
                        className="btn btn-primary"
                      >
                        📥 下载/查看
                      </a>
                    )}
                  </div>
                </div>
              ))
            )}
          </div>
        </section>
      )}

      {activeTab === 'assignments' && (
        <section>
          <div className="card-grid" style={{ gridTemplateColumns: '1fr' }}>
            {currentCourseAssignments.length === 0 ? (
              <div className="empty-state">该课程暂无待办作业</div>
            ) : (
              currentCourseAssignments.map(a => (
                <div className="card" key={a.id} style={{ borderLeft: '4px solid var(--warning-color)' }}>
                  <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                    <div>
                      <h4>{a.title}</h4>
                      <p><strong>截止:</strong> {a.due_date}</p>
                      <p>{a.description}</p>
                    </div>
                    <button 
                      className="btn btn-primary"
                      onClick={() => openSubmitModal(a)}
                    >
                      去提交
                    </button>
                  </div>
                </div>
              ))
            )}
          </div>
        </section>
      )}

      {/* Submit Modal (Reused in Detail View) */}
      {showSubmitModal && (
        <div className="modal-overlay">
          <div className="modal">
            <div className="modal-header">
              <h3>提交作业: {targetAssignmentTitle}</h3>
              <button className="close-button" onClick={closeSubmitModal}>×</button>
            </div>
            <div className="modal-body">
              <div className="form-group">
                <label>作业标题</label>
                <input
                  className="form-control"
                  type="text"
                  value={submissionTitle}
                  onChange={e => setSubmissionTitle(e.target.value)}
                />
              </div>
              <div className="form-group">
                <label>作业内容</label>
                <textarea
                  className="form-control"
                  rows="5"
                  placeholder="在此输入作业内容..."
                  value={content}
                  onChange={e => setContent(e.target.value)}
                />
              </div>
            </div>
            <div className="modal-footer">
              <button className="btn btn-secondary" onClick={closeSubmitModal}>取消</button>
              <button className="btn btn-primary" onClick={handleSubmit}>确认提交</button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}

export default StudentDashboard;


