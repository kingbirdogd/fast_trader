import '/component/warrant/configure.css'

class Configure extends React.Component {
  static propTypes = {
    prefix: PropTypes.string,
    fields: PropTypes.object,
    curModule: PropTypes.string,
    lang: PropTypes.string,
    setFields: PropTypes.func,
    setCurModule: PropTypes.func
  }
  
  constructor(props) {
    super(props)

    this.state = {}
    
    this.handleOnMouseOver = this.handleOnMouseOver.bind(this)
    this.handleChange = this.handleChange.bind(this)
    this.handleDelete = this.handleDelete.bind(this)
    this.handleAdd = this.handleAdd.bind(this)
    this.handleMove = this.handleMove.bind(this)
  }
  
  componentDidMount() {
    $(this.refs.sortable).sortable({
      stop: (event, ui) => this.handleMove(event, ui)
    });
  }
  
  shouldComponentUpdate(nextProps, nextState) {return true}
  
  componentDidUpdate() {}
  
  componentWillUnmount() {}
  
  componentDidCatch() {}
  
  handleOnMouseOver(event) {
    event.preventDefault()
    event.target.focus()
  }
  
  handleChange(event) {
    var id = event.target.attributes.getNamedItem('data-id').value.replace(this.props.prefix, "")
    var {name, value} = event.target
    var fields = this.props.fields
    fields[this.props.curModule][id][name] = formatInput(value)
    this.props.setFields(fields)
  }
  
  handleDelete(event) {
    var id = event.target.attributes.getNamedItem('data-id').value.replace(this.props.prefix, "")
    var fields = this.props.fields
    delete fields[this.props.curModule][id]
    this.props.setFields(fields)
  }
  
  handleAdd(event) {
    var fields = this.props.fields
    var curModule = this.props.curModule
    
    // 如空，react会视为null key报错
    var isEmpty = false
    fields[curModule].map((field, index) => {
      if(!field.name)
        isEmpty = true
    })
    if(!isEmpty) {
      var len = fields[curModule].length
      fields[curModule][len] = {type: "text", name: '', value: '', valid: 'null'}
      this.props.setFields(fields)
    }
  }
  
  handleMove(event, ui) {
    var fields = this.props.fields
    var arr = []
    $(this.refs.sortable).find('li').each(function(key, elem) {
      var type = $(elem).find('[name="type"] option:selected').val()
      var name = $(elem).find('[name="name"]').val()
      var value = $(elem).find('[name="value"]').val()
      var valid = $(elem).find('[name="valid"]').val()
      arr.push({type:type , name:name, value:value, valid:valid})
    })
    fields[this.props.curModule] = arr
    this.props.setFields(fields)
  }
  
  getText(lang) {
    var text = {
      en: {
        add: {name: 'Add'}
      },
      sc: {
        add: {name: '新增'}
      },
      tc: {
        add: {name: '新增'}
      }
    }
    return text[lang]
  }
  
  render() {
    var lang = (this.props.lang) ? this.props.lang : 'sc'
    var text = this.getText(lang)
    var rows = this.props.fields[this.props.curModule].map((field, index) => {
      return(
        <li 
          key={new Date().getTime()+index}  // 必须是唯一键
          className="config">
            <ConfigureRow
              key={this.props.prefix+index}
              id={this.props.prefix+index}
              type={field.type}
              name={field.name}
              value={field.value}
              valid={field.valid}
              lang={this.props.lang}
              handleChange={this.handleChange}
              handleDelete={this.handleDelete}
              handleOnMouseOver={this.handleOnMouseOver}
            />
        </li>
      )
    })
    const add = (
      <div className="config">
        <button
          type="button"
          className="btn btn-sm btn-info"
          onClick={this.handleAdd}>
            <i className="fa fa-plus" aria-hidden="true"></i>{text.add.name}
        </button>
      </div>
    )
    return (
      <React.Fragment>
        <ul ref="sortable">
          {rows}
        </ul>
        {add}
      </React.Fragment>
    )
  }
}