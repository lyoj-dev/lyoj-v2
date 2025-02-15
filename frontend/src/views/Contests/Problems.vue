<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import ProblemCard from '@/components/Problem/Card.vue';
import ContestTitle from '@/components/Contest/Title.vue';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import { useRoute } from 'vue-router';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var url = config.apiBase + "/contests/" + to.params.id;
    var json = await myFetch(url);
    var problems = await myFetch(config.apiBase + "/contests/" + to.params.id + "/problems/list");

    next((e: any) => e.loading({
        data: json.item,
        identity: json.identity,
        signup: json.signup,
        problems: problems.items,
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const route = useRoute();

const item: any = ref({});
const identity: any = ref(0);
const signup: any = ref(false);
const problems: any = ref([]);

function loading(data: any) {
    item.value = data.data;
    identity.value = data.identity;
    signup.value = data.signup;
    problems.value = data.problems;
    loaded.value = true;
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            current="problems" 
            :id="route.params.id" 
            :identity="identity" 
            :signup="signup"
            :endTime="item.starttime + item.duration"
        ></ContestNavigation>
        <ContestTitle
            :id="item.id"
            :title="item.title"
            :startTime="item.starttime"
            :endTime="item.starttime + item.duration"
            :problems="item.problemNumber"
            :signups="item.signups"
        ></ContestTitle>
        <ProblemCard
            v-for="item in problems"
            :status="item.status"
            :id="item.realId"
            :cid="route.params.id"
            :alias="'#' + item.id"
            :title="item.title"
            :tags="item.tags"
            :accepted="item.accepted"
            :total="item.total"
            :difficulty="item.difficulty"
            :allowEdit="item.allowEdit"
            :allowDelete="item.allowDelete"
        ></ProblemCard>
    </div>
</template>