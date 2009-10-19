<?php
// $Id: quiz.help.inc,v 1.2.2.1 2009/05/21 19:02:12 sivaji Exp $
/**
 * Help text.
 *
 * @file
 * quiz help function
 */

/**
 * Handle help calls.
 * @see hook_help()
 */
function _quiz_help($path, $arg) {
  switch ($path) {
    case 'admin/help#quiz':
      return t('
<h3>Description</h3>
<p>The quiz module allows users to administer a quiz, as a sequence of questions, and track the answers given. It allows for the creation of questions (and their answers), and organizes these questions into a quiz. Finally, it provides a mechanism for ensuring question quality through a combination of community revision and moderation. Its target audience includes educational institutions, online training programs, employers, and people who just want to add a fun activity for their visitors to their Drupal site.</p>
<h3>Creating Your First Quiz</h3>
<p>Creating an initial quiz requires three steps:</p>
<ol>
  <li>Create at least one taxonomy vocabulary and assign it to the quiz and question type modules</li>
  <li>Create a series of questions</li>
  <li>Create a quiz based on the series of questions</li>
</ol>
    <h4>Setting Quiz Permissions</h4>
    <p>The quiz module and included multichoice module both have a plethora of permission options.<br />Unless you take care setting your permissions, the quiz module might not do everything you want it to do.</p>
    <h5><strong><a href="@admin-access#module-quiz">Quiz Permissions</a></strong></h5>
    <dl><dt><strong>access quiz</strong></dt>
    <dd>allows users to take quizzes</dd>
    <dt><strong>administer quiz</strong></dt>
    <dd>allows users to edit quizzes</dd>
    <dt><strong>administer quiz configuration</strong></dt>
    <dd>allows users to use global admin settings, as well as override userresults if they don\'t have that permission. Can also delete quizzes</dd>
    <dt><strong>create quiz</strong></dt>
    <dd>Can create a new quiz</dd>
    <dt><strong>user results</strong></dt>
    <dd>Can view other user\'s results.</dd></dl>
    <h5><strong><a href="@admin-access#module-multichoice">Multichoice Permissions</a></strong></h5>
    <dl><dt><strong>allow any number of answers</strong></dt>
    <dd>can submit questions with more than one correct answer.</dd>
    <dt><strong>allow feedback</strong></dt>
    <dd>Can create feedback when creating a new multichoice question.</dd>
    <dt><strong>allow multiple correct answers</strong></dt>
    <dd></dd>
    <dt><strong>allow user titles</strong></dt>
    <dd>Allows users to pick a name for their questions. Otherwise this is auto generated. The question name is never seen on the @quiz.</dd>
    <dt><strong>create multichoice</strong></dt>
    <dd>users can create multichoice questions</dd>
    <dt><strong>edit own multichoice</strong></dt>
    <dd>can edit their own multi-choice questions.</dd></dl>
<h4>Setting up a vocabulary</h4>
<ol>
  <li>If not already enabled, go to the <a href="@admin-modules">Administer >> Site building >> Modules</a> section of the control panel and check the <strong>enable</strong> checkbox to enable the <strong>taxonomy module</strong>.</li>
  <li>If you do not already have a taxonomy <strong>vocabulary</strong> suitable for quizzes, go to <a href="@admin-taxonomy">Administer >> Content management >> Categories</a> and create a vocabulary for quizzes (for example, <strong>Quiz Topics</strong>). Ensure that under <strong>Types</strong>, both <strong>quiz</strong> and all question types (for example, <strong>multichoice</strong>) are selected. Depending on your needs, you may wish to create a hierarchical vocabulary, so that topics can be sub-divided into smaller areas, and/or enable multiple select to associate quizzes and questions with more than one category.</li>
  <li>Add a series of <strong>terms</strong> to the vocabulary to which questions and quizzes can be assigned. For example:
    <ul>
      <li>Literature
        <ul>
          <li>Children\'s books</li>
          <li>Poetry</li>
          <li>Shakespeare</li>
        </ul>
      </li>
      <li>Mathematics
        <ul>
          <li>Algebra</li>
          <li>Trigonometry</li>
          <li>Calculus</li>
        </ul>
      </li>
      <li>Information Technology
        <ul>
          <li>Hardware</li>
          <li>Programming</li>
          <li>Databases</li>
        </ul>
      </li>
  </li>
</ol>
<h4>Creating quiz questions</h4>
<ol>
  <li>Begin by clicking <a href="@create-content">Create content</a>, and then select a question type node (for example, <a href="@multichoice">multichoice</a>)</li>
  <li>Fill out the question form. The presented interface will vary depending on the question type, but for multiple choice questions:
    <dl>
      <dt><strong>Title</strong></dt>
      <dd>Question title. This will be displayed as the heading of the question.</dd>
      <dt><strong>Taxonomy selection</strong></dt>
      <dd>Any taxonomy vocabularies that are assigned to the question type will be displayed.</dd>
      <dt><strong>Question</strong></dt>
      <dd>The actual question text (for example, <strong>What is 2+2?</strong>).</dd>
      <dt><strong>Multiple Answers</strong></dt>
      <dd>Whether or not the question has multiple correct answers, such as a "Select all that apply" question.</dd>
      <dt><strong>Correct</strong></dt>
      <dd>Indicates that given answer is a correct answer.</dd>
      <dt><strong>Answer</strong></dt>
      <dd>An answer choice (for example, <strong>4</strong>). If more answers are required, check <strong>I need more answers</strong> and click the <b>Preview</b> button.</dd>
      <dt><strong>Feedback</strong></dt>
      <dd>Feedback, if supplied, will be provided to the user at the end of the quiz.</dd>
    </dl>
  </li>
  <li>Repeat for each question you would like included on the quiz.</li>
</ol>
<h4>Creating the quiz</h4>
<ol>
  <li>Go to <a href="@create-quiz">Create content >> Quiz</a> to access the quiz creation form.</li>
  <li>Fill out the form to set the @quiz options:
    <dl>
      <dt><strong>Title</strong></dt>
      <dd>Quiz title. This will be displayed as the heading of the quiz.</dd>
      <dt><strong>Taxonomy selection</strong></dt>
      <dd>Any taxonomy vocabularies that are assigned to the quiz type will be displayed. Select from the terms displayed in order to assign the quiz to vocabulary terms.</dd>
      <dt><strong>Shuffle questions</strong></dt>
      <dd>Whether or not to shuffle (randomize) the questions.</dd>
      <dt><strong>Number of takes</strong></dt>
      <dd>Number of takes to allow user. Varies from 1-9 or Unlimited times.</dd>
    </dl>
  </li>
  <li>Once the quiz has been created, click the <b>add questions</b> tab to assign questions to the quiz.</li>
  <li>Select a radio button next to each question indicating if the question should appear (Randomly, Always, or Never), and click <strong></strong>.</li>
  <li>Repeat process until satisfied with question selection.</li>
</ol>
    ', array('@quiz' => QUIZ_NAME, '@admin-access' => url('admin/user/permissions'), '@admin-modules' => url('admin/build/modules'), '@admin-taxonomy' => url('admin/content/taxonomy'), '@create-content' => url('node/add'), '@multichoice' => url('node/add/multichoice'), '@create-quiz' => url('node/add/quiz')));
    case 'node/add#quiz':
      return t('A collection of questions designed to create interactive tests');
    default:
      break;
  }
}
