import Index from '@/views/Index.vue'
import ProblemsList from '@/views/Problems/List.vue'
import ProblemDetails from '@/views/Problems/Details.vue'
import SubmissionsList from '@/views/Submissions/List.vue'
import SubmissionsDetails from '@/views/Submissions/Details.vue'
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
      component: ProblemDetails
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

export function get() {
  var url = location.search;
  var res: any = {};
  if (url.indexOf("?") != -1) {
    var str = url.substr(1);
    var strs = str.split("&");
    for (var i = 0; i < strs.length; i++) 
      res[strs[i].split("=")[0]] = decodeURI(strs[i].split("=")[1]);
  }
  return res;
}

export function sleep(ms: number) {
  return new Promise(resolve => setTimeout(resolve, ms));
}