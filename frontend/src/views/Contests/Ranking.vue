<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import { useRoute } from 'vue-router';
import ContestTitle from '@/components/Contest/Title.vue';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import ContestRank from '@/components/Contest/Rank.vue';
import { i18n } from '@/i18n';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var contest = await myFetch(config.apiBase + "/contests/" + to.params.id);
    var problems = await myFetch(config.apiBase + "/contests/" + to.params.id + "/problems/list");
    var json = await myFetch(config.apiBase + "/contests/" + to.params.id + "/ranking");

    next((e: any) => e.loading({
        contest: contest,
        problems: problems.items,
        data: json
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const my: any = ref({});
const list: any = ref([]);
const contest: any = ref({});
const problems: any = ref([]);
const title: any = ref([]);
const route = useRoute();
const type = ref(0);
const t = i18n.global.t;

function loading(data: any) {
    my.value = data.data.item;
    list.value = data.data.items;
    contest.value = data.contest;
    problems.value = data.problems;
    type.value = contest.value.item.type;
    if (my.value != undefined) list.value = [ my.value ].concat(list.value);

    for (var i = 0; i < contest.value.item.problemNumber; i++) {
        if (i < problems.value.length) title.value.push([ 
            '#' + (i + 1), 
            '/contests/' + route.params.id + '/problems/' + problems.value[i].realId 
        ]);
        else title.value.push([ '#' + (i + 1) ]);
    }

    loaded.value = true;
}

defineExpose({ loading });

function formatTime(time: number) {
    time = Math.floor(time);
    return Math.floor(time / 60).toString().padStart(2, '0') + ':' + (time % 60).toString().padStart(2, '0');
}
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            current="ranking" 
            :id="route.params.id" 
            :identity="contest.identity" 
            :signup="contest.signup"
            :endTime="contest.item.starttime + contest.item.duration"
        ></ContestNavigation>
        <ContestTitle
            :id="contest.item.id"
            :title="contest.item.title"
            :startTime="contest.item.starttime"
            :endTime="contest.item.starttime + contest.item.duration"
            :problems="contest.item.problemNumber"
            :signups="contest.item.signups"
        ></ContestTitle>
        <ContestRank
            :data="[
                t('pages.contests.rank'),
                [ t('pages.contests.username') ],
                [
                    [ t('pages.contests.score') ],
                    [ t('pages.contests.penalty') ]
                ].concat(title)
            ]"
        ></ContestRank>
        <ContestRank
            v-for="my in list"
            :data="[
                '#' + my.id,
                [ my.user, '/users/' + my.uid ],
                [
                    [ 
                        type == 2 && my.penalty ? 
                        my.score + '<span style=\'color: red\'>(' + my.penalty + ')</span>' : 
                        my.score
                    ],
                    [
                        type == 2 ? 
                        formatTime((my.time2 + my.penalty * 20 * 60) / 60) : 
                        Math.round(my.time / 1000 * 10) / 10 + 's'
                    ]
                ].concat(my.info.map((item: any) => 
                    item.sid == 0 ? 
                    [ item.score ] : 
                    [ 
                        type == 2 ? (
                            '<span class=\'d-flex justify-center\' style=\'line-height: 20px;\'>' + 
                                item.score + 
                                (item.penalty ? '<span style=\'color: red\'>(' + item.penalty + ')</span>' : '') +
                            '</span>' +
                            '<span style=\'color: green; font-size: 14px; line-height: 18px;\' class=\'d-flex justify-center\'>' + 
                                formatTime(item.time2 / 60) + 
                            '</span>'
                        ) : item.score, 
                        '/contests/' + route.params.id + '/submissions/' + item.sid
                    ]
                ))
            ]"
        ></ContestRank>
    </div>
</template>