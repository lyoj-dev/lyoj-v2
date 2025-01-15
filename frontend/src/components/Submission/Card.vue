<script setup lang="ts">
import { statusList } from '@/config.ts'
const props = defineProps([ 'id', 'pid', 'problem', 'uid', 'user', 'statusType', 'status', 'score' ]);
console.log(props.statusType);
</script>

<template>
    <v-card class="d-flex SubmissionCard card-radius">
        <router-link :to="'/submissions/' + id" class="SubmissionCard-id">#{{ id }}</router-link>
        <router-link :to="'/users/' + uid" class="SubmissionCard-user">{{ user }}</router-link>
        <router-link :to="'/problems/' + pid" class="SubmissionCard-problem">{{ problem }}</router-link>
        <router-link :to="'/submissions/' + id" :style="'color:' + statusList[statusType].color">
            <v-icon :icon="statusList[statusType].icon" class="SubmissionCard-icon"></v-icon>
            &nbsp;
            {{ score }}
            &nbsp;
            {{ status }}
        </router-link>
    </v-card>
</template>

<style scoped>
.SubmissionCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding-left: 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.SubmissionCard-id {
    width: 10%;
}

.SubmissionCard-user {
    width: 15%;
}

.SubmissionCard-problem {
    width: 45%;
}

.SubmissionCard-icon {
    position: relative;
    top: -1px;
}
</style>