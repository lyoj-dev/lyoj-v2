<script setup lang="ts">
import { difficultyList, maxDifficulty, statusCardList, tagsTypeList } from '@/config';
import { i18n } from '@/i18n';
import { goto, locate } from '@/router';
import { loginAs } from '@/utils';

const t = i18n.global.t;
const props = defineProps([ 'selected', 'id', 'cid', 'alias', 'title', 'tags', 'accepted', 'total', 'difficulty' ]);
const emits = defineEmits([ 'addTag', 'updateSelected', 'deleteProblem', 'cloneProblem', 'rejudge' ]);

function getColor(d: number) {
    for (var i = 1; i < difficultyList.length; i++) {
        if (d / maxDifficulty < difficultyList[i].t) {
            var a = difficultyList[i - 1], b = difficultyList[i];
            var r = (d / maxDifficulty - a.t) / (b.t - a.t) * (b.r - a.r) + a.r;
            var g = (d / maxDifficulty - a.t) / (b.t - a.t) * (b.g - a.g) + a.g;
            var b2 = (d / maxDifficulty - a.t) / (b.t - a.t) * (b.b - a.b) + a.b;
            return 'rgb(' + r + ', ' + g + ', ' + b2 + ')';
        }
    }
}
</script>

<template>
    <v-card class="d-flex ProblemCard card-radius">
        <div
            @click="$emit('updateSelected', !selected)"
            class="ProblemCard-status"
        >
            <v-icon
                :icon="selected ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline'"
            ></v-icon>
        </div>
        <router-link 
            :to="cid != undefined ? '/contests/' + cid + '/problems/' + id : '/problems/' + id" 
            class="ProblemCard-id ellipsis"
        >{{ alias }}</router-link>
        <router-link 
            :to="cid != undefined ? '/contests/' + cid + '/problems/' + id : '/problems/' + id" 
            class="ProblemCard-problem ellipsis"
        >{{ title }}</router-link>
        <div class="ProblemCard-tags d-flex">
            <v-chip 
                v-for="tag in tags" 
                :key="tag" 
                size="small" 
                :color="tagsTypeList[tag.type].color" 
                variant="flat"
                @click="() => emits('addTag', tag.id)"
            >{{ tag.title }}</v-chip>
        </div>
        <div class="ProblemCard-difficulty d-flex justify-center">
            <v-chip 
                :color="getColor(difficulty)" 
                size="small" 
                variant="flat"
                v-if="difficulty != -1"
            >{{ t('pages.problems.difficulty') }}：{{ difficulty }}</v-chip>
        </div>
        <v-progress-linear 
            :model-value="total == 0 ? 0 : accepted / total * 100" 
            :height="15" 
            color="green" 
            bg-color="currentColor" 
            class="ProblemCard-accepted"
        >{{ accepted }} / {{ total }}</v-progress-linear>
        <div class="ProblemCard-actions d-flex justify-center align-center">
            <v-btn 
                class="ProblemCard-actionButton" 
                icon="mdi-restart" 
                size="x-small"
                @click="() => emits('rejudge', id, title)"
            ></v-btn>
            <v-btn 
                class="ProblemCard-actionButton" 
                icon="mdi-pen" 
                size="x-small"
                @click="locate('/problems/' + id + '/edit')"
            ></v-btn>
            <v-btn 
                class="ProblemCard-actionButton" 
                icon="mdi-trash-can" 
                size="x-small"
                @click="() => emits('deleteProblem', id, title)"
            ></v-btn>
            <v-btn 
                class="ProblemCard-actionButton" 
                icon="mdi-content-copy" 
                size="x-small"
                @click="() => emits('cloneProblem', id, title)"
            ></v-btn>
        </div>
    </v-card>
</template>

<style scoped>
.ProblemCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ProblemCard-status {
    display: block;
    width: 3%;
    position: relative;
    top: -0.8px;
    padding: 3px;
    cursor: pointer;
}

.ProblemCard-id {
    padding: 3px;
    width: 7%;
}

.ProblemCard-problem {
    width: calc(35% - 74px);
}

.ProblemCard-tags {
    width: 25%;
    gap: 5px;
    flex-wrap: wrap;
    padding: 10px 0px;
}

.ProblemCard-difficulty {
    width: 10%;
}

.ProblemCard-accepted {
    width: 10%;
    font-size: 12px;
}

.ProblemCard-actions {
    width: calc(10% + 74px);
    gap: 5px;
}

.ProblemCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>