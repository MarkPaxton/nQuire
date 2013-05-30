
$(function() {


  nQuireJsSupport.register('nquireStructureWidget', {
    activityTypes: {
      activity: {
        pi_read_information: {
          title: 'Information',
          multiplicity: '*',
          requires: []
        },
        pi_wiki_notes: {
          title: 'My notes',
          multiplicity: '*',
          requires: []
        },
        pi_hypothesis: {
          title: 'My hypothesis',
          multiplicity: '1',
          requires: []
        },
        pi_hypothesis_conclusion: {
          title: 'My hypothesis conclusion',
          multiplicity: '1',
          requires: ['hypothesis']
        },
        pi_sort_key_questions: {
          title: 'My key questions',
          multiplicity: '1',
          requires: []
        },
        pi_sort_key_answers: {
          title: 'My key answers',
          multiplicity: '1',
          requires: ['keyquestions']
        },
        pi_methodology: {
          title: 'Decide my methodology',
          multiplicity: '*',
          requires: []
        },
        pi_sort_data: {
          title: 'Collect my data',
          multiplicity: '*',
          requires: []
        },
        pi_explore_tool: {
          title: 'Explore a scientific instrument',
          multiplicity: '*',
          requires: []
        },
        pi_data_spreadsheet: {
          title: 'Data spreadsheet analysis',
          multiplicity: '*',
          requires: []
        },
				pi_sort_result_presentations : {
          title: 'Data chart analysis',
          multiplicity: '*',
          requires: []
				}
      },
      phase: {
        introduction: {
          title: 'Introduction',
          activities: ['pi_wiki_notes']
        },
        questions: {
          title: 'My hypothesis',
          activities: ['pihypothesis', 'pi_sort_key_questions']
        },
        methodology: {
          title: 'Methodology',
          activities: ['pi_methodology']
        },
        collectdata: {
          title: 'Data gathering',
          activities: ['pi_sort_data']
        },
        chartanalysedata: {
          title: 'Chart data analysis',
          activities: ['pi_sort_result_presentations']
        },
        spreadsheetanalysedata: {
          title: 'Spreadsheet data analysis',
          activities: ['pi_data_spreadsheet']
        },
        conclusions: {
          title: 'My conclusions',
          activities: ['pi_sort_key_answers', 'pi_hypothesis_conclusion']
        },
        empty: {
          title: 'Empty phase',
          activities: []
        }
      }
    },
    init: function() {
      var self = this;
      $("div[nquire-widget='inquiry-structure']").each(function() {
        $(this).nquireStructureWidget({
          availableActivities: self.activityTypes
        });
      });
    }
  });
});
