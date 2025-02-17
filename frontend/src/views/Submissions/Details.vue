<script lang="ts">
import { computed, defineComponent, ref } from "vue";
import { goto } from "@/router";
import { config } from '@/config.ts'
import NProgress from 'nprogress';
import SubmissionCard from '@/components/Submission/Card.vue'
import SubmissionCode from '@/components/Submission/Code.vue'
import SubmissionCompileInfo from '@/components/Submission/CompileInfo.vue'
import SubmissionData from '@/components/Submission/Data.vue'
import SubmissionSubtask from '@/components/Submission/Subtask.vue'
import ContestNavigation from '@/components/Contest/Navigation.vue'
import { JudgeResultInfo } from "@/shared";
import { myFetch } from "@/utils";
import { onBeforeRouteLeave, useRoute } from "vue-router";

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = await myFetch(config.apiBase + (to.params.cid == undefined ? "" : "/contests/" + to.params.cid) + "/submissions/" + to.params.id);
    var judgeData = await myFetch(config.apiBase + (to.params.cid == undefined ? "" : "/contests/" + to.params.cid) + "/problems/" + data.item.pid + "/data");
    var contest = to.params.cid == undefined ? {} : await myFetch(config.apiBase + "/contests/" + to.params.cid);
    
    next((e: any) => e.loading({
        data: data,
        judgeData: judgeData,
        contest: contest
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script setup lang="ts">
const route = useRoute();
const item: any = ref([]);
const judgeData: any = ref([]);
const loaded = ref(false);
const compiled = ref(false);
const subtasks = computed(() => item.value.subtasks.slice(1));
const info: any = ref({});
const contest: any = ref({});
var ws: WebSocket;

function loading(data: any) {
    item.value = data.data.item;
    info.value = data.data;
    judgeData.value = data.judgeData.items;
    contest.value = data.contest;
    compiled.value = !(item.value.statusType == 9 || item.value.statusType == 10);

    if (item.value.judged == true) {

    } else {
        ws = new WebSocket(config.wsBase + "/submissions/" + item.value.id);
        ws.onmessage = function(e) {
            var json = JSON.parse(e.data);

            var index = 0;
            if (json.type == 0 || json.type == 1) {
                item.value.info = json.info;
                item.value.score = json.score;
                item.value.statusType = json.status;
                item.value.status = JudgeResultInfo[json.status as number];
                if (json.subtasks != undefined) item.value.subtasks = json.subtasks;
            } else if (json.type == 2) {
                item.value.subtasks[json.subtaskId].score = json.score;
                item.value.subtasks[json.subtaskId].status = json.status;
                item.value.score = json.totalScore;
            } else if (json.type == 3) {
                item.value.subtasks[json.subtaskId].datas[json.dataId].time = json.time;
                item.value.subtasks[json.subtaskId].datas[json.dataId].memory = json.memory;
                item.value.subtasks[json.subtaskId].datas[json.dataId].score = json.score;
                item.value.subtasks[json.subtaskId].datas[json.dataId].status = json.status;
                item.value.subtasks[json.subtaskId].datas[json.dataId].info = json.info;
                item.value.subtasks[json.subtaskId].datas[json.dataId].output = json.output;
                item.value.subtasks[json.subtaskId].datas[json.dataId].outputIgnored = json.outputIgnored;
                item.value.status = json.totalStatus;
                item.value.time = json.totalTime;
                item.value.memory = json.totalMemory;
                item.value.score = json.totalScore;
            }
            compiled.value = !(item.value.statusType == 9 || item.value.statusType == 10);

            if (json.type == 1) ws.close();
        };
    }

    loaded.value = true;
}
defineExpose({ loading })

onBeforeRouteLeave((to, from, next) => {
    if (ws != undefined) ws.close();
    next();
});
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            :id="route.params.cid" 
            current="submissions"
            :identity="info.identity"
            :signup="info.signup"
            :endTime="contest.item.starttime + contest.item.duration"
            v-if="route.params.cid != undefined"
        ></ContestNavigation>
        <SubmissionCard
            :id="item.id"
            :cid="route.params.cid"
            :pid="item.pid"
            :uid="item.uid"
            :problem="item.problem"
            :user="item.user"
            :statusType="item.statusType"
            :status="item.status"
            :score="item.score"
        ></SubmissionCard>
        <SubmissionCode
            v-if="item.code != ''"
            :id="item.id"
            :code="item.code"
            :langCode="item.langMode"
            :time="item.time"
            :memory="item.memory"
            :langName="item.langName"
            :date="item.date"
            :allowRejudge="item.rejudge"
        ></SubmissionCode>
        <SubmissionCompileInfo
            :info="item.info"
            v-if="compiled && item.info != undefined && item.info != ''"
        ></SubmissionCompileInfo>
        <v-expansion-panels 
            v-if="compiled && item.subtasks.length"
            style="margin-bottom: 20px;" 
            variant="accordion" 
            static
        >
            <SubmissionData
                v-for="(item2, index) in item.subtasks[0].datas"
                :id="index + 1"
                :data="item2"
                :reversed="0"
                :judgeData="judgeData[0] == undefined ? [] : judgeData[0][index]"
                :pid="item.pid"
            ></SubmissionData>
            <SubmissionSubtask
                v-for="(item2, index) in subtasks"
                :subtask="item2"
                :judgeDatas="judgeData[index + 1]"
                :pid="item.pid"
            ></SubmissionSubtask>
        </v-expansion-panels>
    </div>
</template>