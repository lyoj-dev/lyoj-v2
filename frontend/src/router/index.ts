import Index from '@/views/Index.vue'
import ProblemsList from '@/views/Problems/List.vue'
import ProblemsDetails from '@/views/Problems/Details.vue'
import ProblemsEdit from '@/views/Problems/Edit.vue'
import SubmissionsList from '@/views/Submissions/List.vue'
import SubmissionsDetails from '@/views/Submissions/Details.vue'
import Login from '@/views/Login.vue'
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
      path: '/login',
      name: 'login',
      component: Login
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

export function goto(name: string, query: any) {
	router.push({ name, query });
}

export function locate(url: string) {
  router.push(url);
}