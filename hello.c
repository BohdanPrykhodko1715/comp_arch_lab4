#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>

MODULE_AUTHOR("Bohdan Prykhodko <bohdanprykhodko36@gmail.com>");
MODULE_DESCRIPTION("labwork4");
MODULE_LICENSE("Dual BSD/GPL");

/* Оголошення параметра */
static unsigned int hello_count = 1;
module_param(hello_count, uint, S_IRUGO);  // Параметр доступний для читання

/* Структура для збереження часу */
struct hello_event {
    struct list_head list;
    ktime_t time;
};

/* Голови списку */
static LIST_HEAD(hello_list);

/* Функція ініціалізації */
static int __init hello_init(void)
{
    int i;

    /* Перевірка значення параметра */
    if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
        printk(KERN_WARNING "Hello count is 0 or between 5 and 10. Proceeding with warnings...\n");
    }
    if (hello_count > 10) {
        printk(KERN_ERR "Error: Hello count is greater than 10. Module will not load.\n");
        return -EINVAL;
    }

    /* Виведення повідомлення кілька разів */
    for (i = 0; i < hello_count; i++) {
        printk(KERN_EMERG "Hello, world!\n");

        /* Створення нового елементу списку */
        struct hello_event *event = kmalloc(sizeof(*event), GFP_KERNEL);
        if (!event) {
            printk(KERN_ERR "Failed to allocate memory for hello_event\n");
            return -ENOMEM;
        }

        /* Отримання поточного часу */
        event->time = ktime_get();

        /* Додавання до списку */
        list_add_tail(&event->list, &hello_list);
    }

    return 0;
}

/* Функція деінціалізації */
static void __exit hello_exit(void)
{
    struct hello_event *event, *tmp;

    /* Прохід по списку і виведення часу події */
    list_for_each_entry_safe(event, tmp, &hello_list, list) {
        printk(KERN_INFO "Event time: %lld ns\n", ktime_to_ns(event->time));
        list_del(&event->list);  // Видалення з списку
        kfree(event);  // Звільнення пам'яті
    }
}

/* Реєстрація функцій ініціалізації і деінціалізації */
module_init(hello_init);
module_exit(hello_exit);

