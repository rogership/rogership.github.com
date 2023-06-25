---
layout: post
title: "Como criar um cronograma\ de estudo no Google Sheets\ sincronizado com o Google Calendar"
categories: [Produtividade, Javascript, G Suite, how-to]
---

<nav class="toc-fixed" markdown="1">
* TOC
{:toc}
</nav>

**Sincronizando Google Sheets e Google Calendar como ferramenta de estudo**

As ferramentas do G Suite são extremamentes poderosas e neste artigo mostro como sincronizar o Google Sheets com o Google Calendar para um cronograma de estudo.

## O Appscript do Google Sheets e setups

Assim é definido o <cite>[Appscript][1]</cite>:

>  Apps Script is a cloud-based JavaScript platform that lets you integrate with and automate tasks across Google products.

A figura 1 ilustra como o Appscript é acessado

<p align="center">
  <img src="/images/sheets-calendar-sync/appscript_spreadsheet.png" />
</p>

Crie um novo script, como mostrado na figura 2

<p align="center">
  <img src="/images/sheets-calendar-sync/new_script.png" />
</p>

### Coleta do CalendarID

Colete o CalendarID do calendário específico no Google Calendar

*Inserir as figuras indicando local de verificação do CalendarID*

### Template do Cronograma

Fiquem a vontade para copiar o [Template Cronograma](https://docs.google.com/spreadsheets/d/18yDeBRuJG0KqTidovBiFiWT06EAvJa2YV39_jBgzroI/edit?usp=sharing) 

## O script

Appscript utiliza-se de Javascript, por tanto todo o código é Javascript.

A planilha manipula todas as agendas do calendário sem necessidade de interação manual para gerenciar os eventos do calendário.

O script foi dividido em 3 etapas + Criação do botão na Ui do Google Sheets

1. Setup das Classes to Calendar e Spreadsheets do Appscript
2. Tratar células vazias
3. Laço que tudo acontece
4. Criação do botão UI

### Classes CalendarApp e SpreadsheetApp

~~~Javascript
function syncCalendarEvents() {
  /**
    * 1) Open the Event Calendar, spreadsheet, and get events in calendar.
 **/
  var spreadsheet = SpreadsheetApp.getActiveSheet();
  var calendarId = spreadsheet.getRange("C4").getValue();
  var calendar = CalendarApp.getCalendarById(calendarId);

  //get start day and last day of spreadsheet agenda
  var startDay = spreadsheet.getRange("C10").getValue();
  var values = spreadsheet.getRange("D10:D").getValues();
  var endDay = values.filter(x => x != "").slice(-1)[0]
  
  //console.log("Start Day: " + startDay);
  //console.log("End Day: " + endDay);

  var events = calendar.getEvents(new Date(startDay), new Date(endDay));
  var eventMap = {};

  // Create a map of existing calendar events
  for (var i = 0; i < events.length; i++) {
    var event = events[i];
    var eventTitle = event.getTitle();
    eventMap[eventTitle] = event;

    Logger.log('Event on Calendar: ' + eventMap[eventTitle].getTitle());
  }
~~~

### Handle de células vazias


### O laço que tudo acontece


### A Ui

## Referencias

[1]: https://developers.google.com/apps-script