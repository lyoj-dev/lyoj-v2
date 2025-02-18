import Index from '@/views/Index.vue'
import ProblemsList from '@/views/Problems/List.vue'
import ProblemsDetails from '@/views/Problems/Details.vue'
import ProblemsEdit from '@/views/Problems/Edit.vue'
import SubmissionsList from '@/views/Submissions/List.vue'
import SubmissionsDetails from '@/views/Submissions/Details.vue'
import ContestsList from '@/views/Contests/List.vue'
import ContestsRegister from '@/views/Contests/Register.vue'
import ContestsUnregister from '@/views/Contests/Unregister.vue'
import ContestsIndex from '@/views/Contests/Index.vue'
import ContestsProblemsList from '@/views/Contests/Problems.vue'
import ContestsSubmissionsList from '@/views/Contests/Submissions.vue'
import ContestsRanking from '@/views/Contests/Ranking.vue'
import ContestsEdit from '@/views/Contests/Edit.vue'
import AdminIndex from '@/views/Admin/Index.vue'
import UserDetails from '@/views/User/Details.vue'
import Login from '@/views/Login.vue'
import CasLogin from '@/views/CasLogin.vue'
import Logout from '@/views/Logout.vue'
import { createRouter, createWebHistory } from 'vue-router'
import NProgress from 'nprogress'

export const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      redirect: "index"
    },
    {
      path: '/index',
      name: 'index',
      component: Index,
    },
    {
      path: '/problems/list',
      name: 'problemsList',
      component: ProblemsList
    },
    {
      path: '/problems/:id',
      name: 'problemsDetails',
      component: ProblemsDetails
    },
    {
      path: '/problems/:id/edit',
      name: 'problemsEdit',
      component: ProblemsEdit
    },
    {
      path: '/submissions/list',
      name: 'submissionsList',
      component: SubmissionsList
    },
    {
      path: '/submissions/:id',
      name: 'submissionsDetails',
      component: SubmissionsDetails
    },
    {
      path: '/contests/list',
      name: 'contestsList',
      component: ContestsList
    },
    {
      path: '/contests/:id/register',
      name: 'contestsRegister',
      component: ContestsRegister
    },
    {
      path: '/contests/:id/unregister',
      name: 'contestsUnregister',
      component: ContestsUnregister
    },
    {
      path: '/contests/:id/index',
      name: 'contestIndex',
      component: ContestsIndex
    },
    {
      path: '/contests/:id/problems/list',
      name: 'contestProblemsList',
      component: ContestsProblemsList
    },
    {
      path: '/contests/:cid/problems/:id',
      name: 'contestProblemsDetails',
      component: ProblemsDetails
    },
    {
      path: '/contests/:id/submissions/list',
      name: 'contestSubmissionsList',
      component: ContestsSubmissionsList
    },
    {
      path: '/contests/:cid/submissions/:id',
      name: 'contestSubmissionsDetails',
      component: SubmissionsDetails
    },
    {
      path: '/contests/:id/ranking',
      name: 'contestRanking',
      component: ContestsRanking
    },
    {
      path: '/contests/:id/edit',
      name: 'contestEdit',
      component: ContestsEdit
    },
    {
      path: '/admin/index',
      name: 'adminIndex',
      component: AdminIndex
    },
    {
      path: '/users/:id',
      name: 'userDetails',
      component: UserDetails
    },
    {
      path: '/login',
      name: 'login',
      component: Login
    },
    {
      path: '/cas/login',
      name: 'casLogin',
      component: CasLogin
    },
    {
      path: '/logout',
      name: 'logout',
      component: Logout
    }
  ],
})

router.afterEach((to, from, next) => {
  NProgress.done();
});

export function goto(name: string, query: any, params: any = {}) {
	router.push({ name, query, params });
}

export function locate(url: string) {
  router.push(url);
}