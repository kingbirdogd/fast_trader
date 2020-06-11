import '/component/warrant/configureRow.css'

class ConfigureRow extends React.Component {
  static propTypes = {
    id: PropTypes.string,
    type: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    valid: PropTypes.string,
    handleChange: PropTypes.func,
    handleDelete: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.getText = this.getText.bind(this)
  }

  getText(lang) {
    var text = {
      en: {
        type: {defaultValue: 'type', text: 'text', button: 'button'},
        name: {placeholder: 'name'},
        value: {placeholder: 'title'},
        valid: {defaultValue: 'valid', null: 'null', ucode: 'ucode', code: 'code', string: 'string', number: 'number'},
        delete: {name: 'Delete'}
      },
      sc: {
        type: {defaultValue: '输入类型', text: '文字框', button: '按钮'},
        name: {placeholder: '名称'},
        value: {placeholder: '标题'},
        valid: {defaultValue: '验证类型', null: '不需验证', ucode: '正股编号', code: '权证编号', string: '文字', number: '数字'},
        delete: {name: '删除'}
      },
      tc: {
        type: {defaultValue: '輸入類型', text: '文字框', button: '按鈕'},
        name: {placeholder: '名稱'},
        value: {placeholder: '標題'},
        valid: {defaultValue: '驗證類型', null: '不需驗證', ucode: '正股編號', code: '權證編號', string: '文字', number: '數字'},
        delete: {name: '刪除'}
      }
    }
    return text[lang]
  }
  
  render() {
    var lang = (this.props.lang) ? this.props.lang : 'sc'
    var text = this.getText(lang)
    
    return(
      <React.Fragment>
        <i className="fa fa-sort" aria-hidden="true"></i>
        
        <select 
          className="selector"
          name="type"
          data-id={this.props.id}
          value={this.props.type}
          onChange={this.props.handleChange}>
            <option value="" disabled defaultValue> {text.type.defaultValue} </option>
            <option value="text"> {text.type.text} </option>
            <option value="button"> {text.type.button} </option>
        </select>
        
        <input
          className="field"
          name="name"
          type="text"
          placeholder={text.name.placeholder}
          autoComplete="off"
          data-id={this.props.id}
          value={this.props.name}
          onChange={this.props.handleChange}
          onMouseOver={this.props.handleOnMouseOver}
        />
        
        <input
          className="field"
          name="value"
          type="text"
          placeholder={text.value.placeholder}
          autoComplete="off"
          data-id={this.props.id}
          value={this.props.value}
          onChange={this.props.handleChange}
          onMouseOver={this.props.handleOnMouseOver}
        />
        
        <select 
          className="selector"
          name="valid"
          data-id={this.props.id}
          value={this.props.valid}
          onChange={this.props.handleChange}>
            <option value="" disabled defaultValue> {text.valid.defaultValue} </option>
            <option value="null"> {text.valid.null} </option>
            <option value="ucode"> {text.valid.ucode} </option>
            <option value="code"> {text.valid.code} </option>
            <option value="string"> {text.valid.string} </option>
            <option value="number"> {text.valid.number} </option>
        </select>
        
        <button
          className="btn btn-sm btn-danger"
          type="button"
          data-id={this.props.id}
          onClick={this.props.handleDelete}>
            <i className="fa fa-times" aria-hidden="true"></i>{text.delete.name}
        </button>
      </React.Fragment>
    )
  }
}
