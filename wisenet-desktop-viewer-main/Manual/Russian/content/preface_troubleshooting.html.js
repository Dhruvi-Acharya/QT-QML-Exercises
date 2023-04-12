var preface_troubleshooting =  '<div class="item_div_cont" data-content="p0103">'+
''+
'    <h3><a class="heading-offset-anchor" id="p0103"></a>Устранение неполадок</h3>'+
'    <br><br>'+
'    <ul>'+
'        <li>Устройство не подключено.</li>'+
'        <dd>Введите в веб-браузере сведения об IP-адресе и порте, которые вводились во время регистрации устройства в Wisenet Viewer, и проверьте, откроется ли страница.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>При подключении с использованием http://IP вводите порт 80 (стандартный порт) в качестве значения порта.</li>'+
'                <li>При подключении с использованием http://IP вводите порт 443 (стандартный порт) в качестве значения порта.</li>'+
'            </ol>'+
'        </dd>'+
'        <dd>Для усиления безопасности Wisenet Viewer сначала подключается по протоколу HTTPS, если в сведениях об устройстве есть соответствующие данные.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>Некоторые из старых прошивок видеорегистратора могут передавать неверный номер порта HTTPS, что препятствует подключению устройства.</li>'+
'                <li>Некоторые из старых прошивок видеорегистратора могут не поддерживать подключение через HTTPS, что препятствует подключению устройства.</li>'+
'            </ol>'+
'        </dd>'+
''+
'        <li>Тогда оно не появится в списке при поиске устройств.</li>'+
'        <dd>Проверьте, правильно ли отображается список устройств в диспетчере устройств.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>Если список устройств правильно отображается в диспетчере устройств, но не в Wisenet Viewer, проверьте, установлено ли разрешению на выполнение значение <b>Нет</b> при первом запуске Wisenet Viewer.</li>'+
'            </ol>'+
'        <dd>Проверьте, зарегистрирована ли программа Wisenet Viewer в брандмауэре Windows и не блокирует ли он ее. </dd>'+
''+
'        <li>Потребуется детализированная история журнала Wisenet Viewer.</li>'+
'        <dd>Если проблема сохранится при использовании Wisenet Viewer и вам необходимо будет обратиться в центр обслуживания клиентов, составьте и предоставьте системный журнал. Получив системный журнал в виде сжатого файла, специалисты центра обслуживания клиентов смогут точно диагностировать и устранить проблему.</dd>'+
'        <dd class="none-list-ol">'+
'            <ol>'+
'                <li>Дополнительные сведения о составлении системного журнала см. в разделе <label class="viewbtns" data-id="setup_system_systemlog" onclick="location.href=\'#p100205\';">Настройка ведения системных журналов</label>.</li>'+
'            </ol>'+
'        </dd>'+
''+
'        <li>Какая информация отображается при нажатии <i class="img_preveal_p_i"><img src="./image/sub/i12.svg" /></i>　　кнопки на панели функциональных кнопок?</li>'+
'        <dd>Будет показана текущая принимаемая частота кадров. Частота кадров показывает число кадров, которые проходят буферизацию для видео.</dd>'+
''+
'        <li>Я хотел воспроизвести записанное видео двойным щелчком по событию сразу после того, как событие появилось на панели событий, но видео не запустилось.</li>'+
'        <dd>Обычно видеорегистратор сохраняет видео на жестком диске после накопления 2 МБ или 4 МБ данных (может отличаться в зависимости от платформы видеорегистратора). Видео с меньшим объемом данных сохраняются на жестком диске с задержкой до 15 секунд. Видео может воспроизводиться на клиенте только после его сохранения на жестком диске.</dd>'+
''+
'        <li>Поддерживаются ли более ранние модели цифровых видеорегистраторов (DVR)?</li>'+
'        <dd>Программа Wisenet Viewer совместима с цифровыми и сетевыми видеорегистраторами (NVR) с поддержкой SUNAPI 2.3.1 и более поздних версий. Более ранние модели цифровых видеорегистраторов не поддерживают протокол SUNAPI, поэтому они несовместимы с Wisenet Viewer. Однако если после обновления прошивки она станет поддерживать SUNAP 2.3.1 или более поздней версии, она будет совместима с Wisenet Viewer.</dd>'+
''+
'        <li>Какая информация обрабатывается при резервном копировании и восстановлении через меню «Настройки»?</li>'+
'        <dd>Резервное копирование и восстановление осуществляется для закладок, каналов, устройств, расписаний событий, групп устройств, раскладок, пользователей, групп пользователей, веб-страниц, правил событий и адресов электронной почты.</dd>'+
''+
'        <li>Что означает ID продукта на экране регистрации устройства вручную?</li>'+
'        <dd>Если при регистрации устройства выбран параметр DDNS/P2P, необходимо будет ввести ID устройства. ID продукта регистрируется на сайте DDNS для установления удаленного подключения и используется при регистрации устройства в DDNS.</dd>'+
''+
'        <li>Я хочу задать скорость воспроизведения видеозаписи.</li>'+
'        <dd>Для перемотки вперед вы можете пользоваться ползунком управления скоростью воспроизведения на временной шкале. Если вы хотите задать скорость воспроизведения, выберите нужную скорость в выпадающем списке рядом с ползунком скорости воспроизведения.</dd>'+
'    </ul>'+
''+
'</div>';